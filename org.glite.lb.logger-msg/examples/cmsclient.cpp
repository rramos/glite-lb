/*
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *	 http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <decaf/lang/Thread.h>
#include <decaf/lang/Runnable.h>
#include <decaf/util/concurrent/CountDownLatch.h>
#include <activemq/core/ActiveMQConnectionFactory.h>
#include <activemq/core/ActiveMQConnection.h>
#include <activemq/transport/DefaultTransportListener.h>
#include <activemq/library/ActiveMQCPP.h>
#include <decaf/lang/Integer.h>
#include <activemq/util/Config.h>
#include <decaf/util/Date.h>
#include <cms/Connection.h>
#include <cms/Session.h>
#include <cms/TextMessage.h>
#include <cms/BytesMessage.h>
#include <cms/MapMessage.h>
#include <cms/ExceptionListener.h>
#include <cms/MessageListener.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <cstring>

using namespace activemq;
using namespace activemq::core;
using namespace activemq::transport;
using namespace decaf::lang;
using namespace decaf::util;
using namespace decaf::util::concurrent;
using namespace cms;
using namespace std;

////////////////////////////////////////////////////////////////////////////////
class SimpleAsyncConsumer : public ExceptionListener,
							public MessageListener,
							public DefaultTransportListener {

private:

	Connection* connection;
	Session* session;
	Destination* destination;
	MessageConsumer* consumer;
	bool useTopic;
	bool clientAck;
	std::string brokerURI;
	std::string destURI;
	FILE * outputfile;

public:

	SimpleAsyncConsumer( const std::string& brokerURI,
						 const std::string& destURI,
						 bool useTopic = false,
						 bool clientAck = false ) {
		connection = NULL;
		session = NULL;
		destination = NULL;
		consumer = NULL;
		this->useTopic = useTopic;
		this->brokerURI = brokerURI;
		this->destURI = destURI;
		this->clientAck = clientAck;
	}

	virtual ~SimpleAsyncConsumer() throw() {
		this->cleanup();
	}

	void close() {
		this->cleanup();
	}

	void runConsumer() {

		try {

			// Create a ConnectionFactory
			ActiveMQConnectionFactory* connectionFactory =
				new ActiveMQConnectionFactory( brokerURI );

			// Create a Connection
			connection = connectionFactory->createConnection();
			delete connectionFactory;

			ActiveMQConnection* amqConnection = dynamic_cast<ActiveMQConnection*>( connection );
			if( amqConnection != NULL ) {
				amqConnection->addTransportListener( this );
			}

			connection->start();

			connection->setExceptionListener(this);

			// Create a Session
			if( clientAck ) {
				session = connection->createSession( Session::CLIENT_ACKNOWLEDGE );
			} else {
				session = connection->createSession( Session::AUTO_ACKNOWLEDGE );
			}

			// Create the destination (Topic or Queue)
			if( useTopic ) {
				destination = session->createTopic( destURI );
			} else {
				destination = session->createQueue( destURI );
			}

			// Create a MessageConsumer from the Session to the Topic or Queue
			consumer = session->createConsumer( destination );
			consumer->setMessageListener( this );

		} catch (CMSException& e) {
			e.printStackTrace();
		}
	}

	// Called from the consumer since this class is a registered MessageListener.
	virtual void onMessage( const Message* message ) throw() {

		static int count = 0;

		try
		{
			count++;
			const TextMessage* textMessage =
				dynamic_cast< const TextMessage* >( message );
			string text = "";

			if( textMessage != NULL ) {
				text = textMessage->getText();
			} else {
				text = "NOT A TEXTMESSAGE!";
			}

			if( clientAck ) {
				message->acknowledge();
			}

			fprintf(outputfile, "Message #%d Received: %s\n", count, text.c_str() );
			fflush(outputfile);
		} catch (CMSException& e) {
			e.printStackTrace();
		} catch (Exception& e) {
			e.printStackTrace();
		} catch (...) {
			fprintf(stderr, "Unknown Exception occurred.\n");
		}
	}

	// If something bad happens you see it here as this class is also been
	// registered as an ExceptionListener with the connection.
	virtual void onException( const CMSException& ex AMQCPP_UNUSED ) {
		fprintf(stderr, "CMS Exception occurred.  Shutting down client.\n");
		//exit(1);
	}

	virtual void transportInterrupted() {
		fprintf(stderr, "The Connection's Transiort has been Interrupted.\n");
	}

	virtual void transportResumed() {
		fprintf(stderr, "The Connection's Transport has been Restored.\n");
	}

	virtual void setOutfile(FILE * setout) {
		outputfile=setout;
	}

private:

	void cleanup(){

		//*************************************************
		// Always close destination, consumers and producers before
		// you destroy their sessions and connection.
		//*************************************************

		// Destroy resources.
		try{
			if( destination != NULL ) delete destination;
		}catch (CMSException& e) {}
		destination = NULL;

		try{
			if( consumer != NULL ) delete consumer;
		}catch (CMSException& e) {}
		consumer = NULL;

		// Close open resources.
		try{
			if( session != NULL ) session->close();
			if( connection != NULL ) connection->close();
		}catch (CMSException& e) {}

		// Now Destroy them
		try{
			if( session != NULL ) delete session;
		}catch (CMSException& e) {}
		session = NULL;

		try{
			if( connection != NULL ) delete connection;
		}catch (CMSException& e) {}
		connection = NULL;
	}
};

////////////////////////////////////////////////////////////////////////////////
int main(int argc AMQCPP_UNUSED, char* argv[] AMQCPP_UNUSED) {
	char *argbrokerURI = NULL;
	char *argdestURI = NULL;
	char *outputfile = NULL;
	FILE * outfile;
	int i;


	for(i=1; i<argc; i++) 
		if((!strcmp(argv[i],"-o")) && (i<argc)) outputfile = argv[++i];
		else 	if(!argbrokerURI) argbrokerURI=argv[i];
			else argdestURI=argv[i];
	if (!(outfile = fopen (outputfile,"w"))) outfile = stdout;

	if (!argdestURI) {
		printf("Usage: %s [-o <output_file>] <broker> <topic>\n", argv[0]);
		return 1;
	}

	activemq::library::ActiveMQCPP::initializeLibrary();

	fprintf(outfile, "=====================================================\n");
	fprintf(outfile, "Starting the example:\n");
	fprintf(outfile, "-----------------------------------------------------\n");

	// Set the URI to point to the IPAddress of your broker.
	// add any optional params to the url to enable things like
	// tightMarshalling or tcp logging etc.  See the CMS web site for
	// a full list of configuration options.
	//
	//  http://activemq.apache.org/cms/
	//
	// Wire Format Options:
	// =====================
	// Use either stomp or openwire, the default ports are different for each
	//
	// Examples:
	//	tcp://127.0.0.1:61616					  default to openwire
	//	tcp://127.0.0.1:61616?wireFormat=openwire  same as above
	//	tcp://127.0.0.1:61613?wireFormat=stomp	 use stomp instead
	//
	std::string brokerURI = "failover:(tcp://";
	brokerURI += argbrokerURI;
	brokerURI += ")";

//	std::string brokerURI =
//		"failover:(tcp://harad.ics.muni.cz:61616"
//		"?wireFormat=openwire"
//		"&connection.useAsyncSend=true"
//		"&transport.commandTracingEnabled=true"
//		"&transport.tcpTracingEnabled=true"
//		"&wireFormat.tightEncodingEnabled=true"
//		")";

	//============================================================
	// This is the Destination Name and URI options.  Use this to
	// customize where the consumer listens, to have the consumer
	// use a topic or queue set the 'useTopics' flag.
	//============================================================
	std::string destURI = argdestURI; //?consumer.prefetchSize=1";

	//============================================================
	// set to true to use topics instead of queues
	// Note in the code above that this causes createTopic or
	// createQueue to be used in the consumer.
	//============================================================
	bool useTopics = true;

	//============================================================
	// set to true if you want the consumer to use client ack mode
	// instead of the default auto ack mode.
	//============================================================
	bool clientAck = false;

	// Create the consumer
	SimpleAsyncConsumer consumer( brokerURI, destURI, useTopics, clientAck );

	consumer.setOutfile(outfile);

	// Start it up and it will listen forever.
	consumer.runConsumer();

	// Wait to exit.
	fprintf(stdout, "Press 'q' to quit\n");
	while( std::cin.get() != 'q') {}

	// All CMS resources should be closed before the library is shutdown.
	consumer.close();

	fprintf(outfile, "-----------------------------------------------------\n");
	fprintf(outfile, "Finished with the example.\n");
	fprintf(outfile, "=====================================================\n");

	if (outputfile) fclose(outfile);

	activemq::library::ActiveMQCPP::shutdownLibrary();
}


