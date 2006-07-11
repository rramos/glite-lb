#!/bin/bash


test_glite_location()
{
	GLITE_LOCATION_VAR=${GLITE_LOCATION_VAR:-/tmp}

	[ -n "$GLITE_LB_PROXY_SOCK_PREFIX" ] && \
		export GLITE_WMS_LBPROXY_STORE_SOCK=$GLITE_LB_PROXY_SOCK_PREFIX\store.sock

	if [ -z "$GLITE_LOCATION" ];
	then
        	echo "GLITE_LOCATION not set! Exiting.";
	        exit 1;
	fi
}

my_echo()
{
	echo $1 1>&2
}


start_bkserver()
{
	# production version - usable only when starting as root 
	# $GLITE_LOCATION/etc/init.d/glite-lb-bkserverd start

	# workaround
	[ -n "$GLITE_LB_SERVER_PIDFILE" ] && pidfile=$GLITE_LB_SERVER_PIDFILE ||
	        pidfile=$GLITE_LOCATION_VAR/glite-lb-bkserverd.pid

	[ -n "$GLITE_HOST_CERT" -a -n "$GLITE_HOST_KEY" ] &&
                creds="-c $GLITE_HOST_CERT -k $GLITE_HOST_KEY"

	if test -z "$creds"; then
                if [ -r /etc/grid-security/hostkey.pem -a -r /etc/grid-security/hostcert.pem ]; then
                        creds="-c /etc/grid-security/hostcert.pem -k /etc/grid-security/hostkey.pem"
                fi
        fi
	[ -z "$creds" ] && \
		echo WARNING: No credentials specified. Using default lookup which is dangerous. >&2

	[ -z "$GLITE_LB_EXPORT_JPREG_MAILDIR" ] && \
		GLITE_LB_EXPORT_JPREG_MAILDIR=/tmp/jpreg
        maildir="--jpreg-dir $GLITE_LB_EXPORT_JPREG_MAILDIR"
        [ -d "$GLITE_LB_EXPORT_JPREG_MAILDIR" ] || \
		mkdir -p "$GLITE_LB_EXPORT_JPREG_MAILDIR" 

	[ -n "$GLITE_LB_SERVER_PORT" ] && port="-p $GLITE_LB_SERVER_PORT"
        [ -n "$GLITE_LB_SERVER_WPORT" ] && wport="-w $GLITE_LB_SERVER_WPORT"
        [ -n "$GLITE_LB_SERVER_TRANSACTION" ] && trans="-b $GLITE_LB_SERVER_TRANSACTION"

	if [ -n "$1" ]; then
		sink="--perf-sink $1"
	else
		sink=""
	fi 	

	echo -n "Starting glite-lb-bkserver ..."
	$GLITE_LOCATION/bin/glite-lb-bkserverd \
		$creds -i $pidfile $port $wport $maildir $sink $trans\
		&& echo " done" || echo " FAILED"
	echo
}


stop_bkserver()
{
	echo
	if [ -f $pidfile ]; then
                pid=`cat $pidfile`
                kill $pid
                echo -n Stopping glite-lb-bkserverd \($pid\) ...
                try=0
                while ps p $pid >/dev/null 2>&1; do
                        sleep 1;
			kill $pid 2>/dev/null
                        try=`expr $try + 1`
                        if [ $try = 20 ]; then
                                echo " giving up after $try retries"
                                return 1
                        fi
                done
                echo " done"
                rm -f $pidfile
		rm -rf $GLITE_LB_EXPORT_JPREG_MAILDIR
        else
                echo $pidfile does not exist - glite-lb-bkserverd not running? >&2 
                return 1
        fi

}


start_proxy()
{
	[ -n "$GLITE_LB_PROXY_PIDFILE" ] && proxy_pidfile=$GLITE_LB_PROXY_PIDFILE ||
		proxy_pidfile=$GLITE_LOCATION_VAR/glite-lb-proxy.pid

	[ -n "$GLITE_LB_PROXY_SOCK_PREFIX" ] && proxy_sock_prefix="-p $GLITE_LB_PROXY_SOCK_PREFIX" ||
		proxy_sock_prefix=""
	
	[ -n "$1" ] && sink="-K $1" || sink_mode=

	echo -n Starting glite-lb-proxy ...
	$GLITE_LOCATION/bin/glite-lb-proxy \
                -i $proxy_pidfile $proxy_sock_prefix $sink \
		&& echo " done" || echo " FAILED"	
	echo
}

stop_proxy()
{
	echo 
	if [ -f $proxy_pidfile ]; then
                pid=`cat $proxy_pidfile`
                kill $pid
                echo -n Stopping glite-lb-proxy \($pid\) ...
                try=0
                while ps p $pid >/dev/null 2>&1; do
                        sleep 1;
			kill $pid 2>/dev/null
                        try=`expr $try + 1`
                        if [ $try = 20 ]; then
                                echo " giving up after $try retries"
                                return 1
                        fi
                done
                echo " done"
                rm -f $proxy_pidfile
		rm -rf $GLITE_LB_EXPORT_JPREG_MAILDIR
        else
                echo $proxy_pidfile does not exist - glite-lb-proxy not running? >&2
                return 1
        fi
}


#  - Test types -
#
#   i) normally procesed by server & proxy
#  ii) server replies immedia success
# iii) proxy replies immediate succes
#   a) current implementation
#   b) connection pool
#   c) parallel communication

test_ai()
{
	dest=
	[ -z "$1" ] && echo "test_ai() - wrong params" && return
	[ "$1" = "proxy" ] && dest="-x"
	[ "$1" = "server" ] && dest="-m $HOST:$PORT"
	[ -z "$dest" ] && echo "test_ai() - wrong params" && return
	
	my_echo "================================================================"
	my_echo "Testing LB $1 with sink_mode ${sink_mode[$i]}"

	# single registration
	#
	my_echo "-n single registration ..."
	ai_sr_lb=`$GLITE_LOCATION/sbin/glite-lb-perftest_jobreg $dest`
	mega_actions_per_day=`echo "scale=6; 86400/$ai_sr_lb/1000000" | bc`
	my_echo ". $ai_sr_lb seconds ($mega_actions_per_day GU)"

	# average single registration (100 samples)
	#
	my_echo "-n average single registration (100 samples) ..."
	ai_100sr_lb=`$GLITE_LOCATION/sbin/glite-lb-perftest_jobreg $dest -N 100`
	ai_avg_sr_lb=`echo "scale=6; $ai_100sr_lb/100" |bc`
	mega_actions_per_day=`echo "scale=6; 86400/$ai_avg_sr_lb/1000000" | bc`
	my_echo ". $ai_avg_sr_lb seconds ($mega_actions_per_day GU)"

	# 1000 nodes DAG registration
	#
	my_echo "-n 1000 nodes DAG registration ..."
	ai_dag1000_lb=`$GLITE_LOCATION/sbin/glite-lb-perftest_jobreg $dest -n 1000`
	mega_actions_per_day=`echo "scale=6; 86400/$ai_dag1000_lb/1000000*1001" | bc`
	my_echo ". $ai_dag1000_lb seconds ($mega_actions_per_day GU)"

	# 5000 nodes DAG registration
	#
	my_echo "-n 5000 nodes DAG registration ..."
	ai_dag5000_lb=`$GLITE_LOCATION/sbin/glite-lb-perftest_jobreg $dest -n 5000`
	mega_actions_per_day=`echo "scale=6; 86400/$ai_dag5000_lb/1000000*5001" | bc`
	my_echo ". $ai_dag5000_lb seconds ($mega_actions_per_day GU)"

	# 10000 nodes DAG registration
	#
	my_echo "-n 10000 nodes DAG registration ..."
	ai_dag10000_lb=`$GLITE_LOCATION/sbin/glite-lb-perftest_jobreg $dest -n 10000`
	mega_actions_per_day=`echo "scale=6; 86400/$ai_dag10000_lb/1000000*10001" | bc`
	my_echo ". $ai_dag10000_lb seconds ($mega_actions_per_day GU)"

}

################################################################################

unset creds port
HOST=`hostname -f`
PORT=${GLITE_LB_SERVER_PORT:-9000}; 
sink_mode[0]=GLITE_LB_SINK_NONE
sink_mode[1]=GLITE_LB_SINK_PARSE
sink_mode[2]=GLITE_LB_SINK_STORE
sink_mode[3]=GLITE_LB_SINK_STATE
sink_mode[4]=GLITE_LB_SINK_SEND

test_glite_location;

for i in 1 2 3 4; do

	start_proxy $i
	test_ai proxy $i;
	stop_proxy

	start_bkserver $i;
	test_ai server $i;
	stop_bkserver;
done

echo "__________"
echo "GU (goal units) are millons of registrations per day, where registration is"
echo "registration of job or subjob by client or server"
echo
