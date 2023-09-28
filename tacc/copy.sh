#!/bin/bash
set -x

echo "TACC: job $SLURM_JOB_ID execution at: `date`"

# our node name
NODE_HOSTNAME=`hostname -s`

# HPC system target. Used as DCV host
HPC_HOST=`hostname -d`

echo "TACC: running on node $NODE_HOSTNAME on $HPC_HOST"

TAP_FUNCTIONS="/share/doc/slurm/tap_functions"
if [ -f ${TAP_FUNCTIONS} ]; then
  . ${TAP_FUNCTIONS}
else
  echo "TACC:"
  echo "TACC: ERROR - could not find TAP functions file: ${TAP_FUNCTIONS}"
  echo "TACC: ERROR - Please submit a consulting ticket at the TACC user portal"
  echo "TACC: ERROR - https://portal.tacc.utexas.edu/tacc-consulting/-/consult/tickets/create"
  echo "TACC:"
  echo "TACC: job $SLURM_JOB_ID execution finished at: $(date)"
  exit 1
fi

# confirm DCV server is alive
DCV_SERVER_UP=`systemctl is-active dcvserver`
if [ $DCV_SERVER_UP == "active" ]; then
  SERVER_TYPE="DCV"
else
  echo "TACC:"
  echo "TACC: ERROR - could not confirm dcvserver active, systemctl returned '$DCV_SERVER_UP'"
  SERVER_TYPE="VNC"
fi

# if X0 socket exists, then DCV will use a higher X display number and ruin our day
# therefore, cowardly bail out and appeal to an admin to fix the problem
if [ -f /tmp/.X11-unix/X0 ]; then
  echo "TACC:"
  echo "TACC: ERROR - X0 socket already exists. DCV script will fail."
  echo "TACC: ERROR - Please submit a consulting ticket at the TACC user portal"
  echo "TACC: ERROR - https://portal.tacc.utexas.edu/tacc-consulting/-/consult/tickets/create"
  echo "TACC:"
  SERVER_TYPE="VNC"
fi

# create an X startup file in /tmp
# source xinitrc-common to ensure xterms can be made
# then source the user's xstartup if it exists
XSTARTUP="/tmp/dcv-startup-$USER"
cat <<- EOF > $XSTARTUP
#!/bin/sh

unset SESSION_MANAGER
unset DBUS_SESSION_BUS_ADDRESS
. /etc/X11/xinit/xinitrc-common
exec startxfce4
EOF

### NOTE: an ampersand after "exec startxfce4" can break DCV.
### This ampersand was found in sal's $HOME/.vnc/xstartup for some reason, so disable for all just in case
# if [ -x $HOME/.vnc/xstartup ]; then
#   cat $HOME/.vnc/xstartup >> $XSTARTUP
# else
#   echo "exec startxfce4" >> $XSTARTUP
# fi
chmod a+rx $XSTARTUP

if [ "x${SERVER_TYPE}" == "xDCV" ]; then
  # create DCV session for this job
  DCV_HANDLE="${AGAVE_JOB_ID}-session"
  dcv create-session --owner ${AGAVE_JOB_OWNER} --init=$XSTARTUP $DCV_HANDLE

  # Wait a few seconds for dcvserver to spin up
  sleep 5;

  if ! `dcv list-sessions 2>&1 | grep -q ${DCV_HANDLE}`; then
    echo "TACC:"
    echo "TACC: WARNING - could not find a DCV session for this job"
    echo "TACC: WARNING - This could be because all DCV licenses are in use."
    echo "TACC: WARNING - Failing over to VNC session."
    echo "TACC: "
    echo "TACC: If you rarely receive a DCV session using this script, "
    echo "TACC: please submit a consulting ticket at the TACC user portal:"
    echo "TACC: https://portal.tacc.utexas.edu/tacc-consulting/-/consult/tickets/create"
    echo "TACC: "

    SERVER_TYPE="VNC"
  else
    LOCAL_PORT=8443  # default DCV port
    DISPLAY=":0"
  fi
fi

if [ "x${SERVER_TYPE}" == "xVNC" ]; then

  VNCSERVER_BIN=`which vncserver`
  echo "TACC: using default VNC server $VNCSERVER_BIN"

  TAPIS_PASS=`which vncpasswd`
  echo -n ${AGAVE_JOB_ID} > tapis_uuid
  ${TAPIS_PASS} -f < tapis_uuid > vncp.txt

  # launch VNC session
  VNC_DISPLAY=`$VNCSERVER_BIN -rfbauth vncp.txt $@ 2>&1 | grep desktop | awk -F: '{print $3}'`
  echo "TACC: got VNC display :$VNC_DISPLAY"

  if [ x$VNC_DISPLAY == "x" ]; then
    echo "TACC: "
    echo "TACC: ERROR - could not find display created by vncserver: $VNCSERVER"
    echo "TACC: ERROR - Please submit a consulting ticket at the TACC user portal:"
    echo "TACC: ERROR - https://portal.tacc.utexas.edu/tacc-consulting/-/consult/tickets/create"
    echo "TACC: "
    echo "TACC: job $SLURM_JOB_ID execution finished at: `date`"
    exit 1
  fi

  VNC_PORT=`expr 5900 + $VNC_DISPLAY`
  LOCAL_PORT=5902
  DISPLAY=":${VNC_DISPLAY}"
fi
