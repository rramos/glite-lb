#!/bin/sh

[ -f /etc/default/glite-lb ] && . /etc/default/glite-lb

if [ "x$GLITE_GSS_MECH" != "xkrb5" ]
then
    exit 0
fi

GLITE_USER=${GLITE_USER:-'glite'}
GLITE_HOME=`getent passwd ${GLITE_USER} | cut -d: -f6`

# KRB5_KTNAME here is different than KRB5_KTNAME in startup scripts,
# here we use system wide keytab to obtain tickets for interlogd, 
# which uses them as a client connecting to LB server
# (keytab in startup scripts is used by logd acting as a server)
KRB5_KTNAME="/etc/krb5.keytab"

ticket=${GLITE_HOME}/krb5cc_lb
KRB5CCNAME="FILE:$ticket"

[ -d $GLITE_HOME ] || mkdir -p $GLITE_HOME

KTUTIL=${KTUTIL:-"/usr/sbin/ktutil"}
HOSTNAMECMD=${HOSTNAMECMD:-"/bin/hostname"}
KINIT=${KINIT:-"/usr/bin/kinit"}
KLIST=${KLIST:-"/usr/bin/klist"}

[ -x ${KTUTIL} ] && [ -x ${HOSTNAMECMD} ] && [ -x ${KINIT} ] && [ -x ${KLIST} ] || exit 1

PRINCIPAL=`${KTUTIL} list 2>/dev/null | grep $($HOSTNAMECMD -f) | grep host | awk '//{print $3}'|uniq`

[ -z "${PRINCIPAL}" ] && exit 1

try=5
while [ $try -gt 0 ]
do 
  KRB5CCNAME="FILE:${ticket}0" ${KINIT} -k $PRINCIPAL

  if  ${KLIST} -c  "FILE:${ticket}0" -s 
  then
      chown $GLITE_USER:$GLITE_GROUP ${ticket}0
      mv ${ticket}0 $ticket
      exit 0
  else 
      try=$(( $try - 1 ))
      sleep 10
  fi
done

