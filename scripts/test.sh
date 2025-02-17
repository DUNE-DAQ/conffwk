#! /bin/sh

###############################################################################

test_schema_file="/tmp/test.schema.$$.xml"
test_data_file="/tmp/test.data.$$.xml"

rm -f ${test_schema_file} ${test_data_file}
cmd="oks_tutorial ${test_schema_file} ${test_data_file}"

echo 'GENERATE FILES FOR TEST ...'
echo " * execute \"${cmd}\""

${cmd} > /dev/null
if [ ! -f ${test_schema_file} ] ; then
  echo 'ERROR: oks_tutorial failed, can not produce schema file for test'
  exit 1
fi

echo ' * done'
echo ''


###############################################################################

echo '**********************************************************************'
echo '******************** tests using oksconflibs plug-in *******************'
echo '**********************************************************************'
echo ''

echo "${1}/conffwk_time_test -d oksconflibs:daq/segments/setup-initial.data.xml"
echo ''

if ${1}/conffwk_time_test -d "oksconflibs:daq/segments/setup-initial.data.xml"
then
  echo '' 
  echo 'conffwk_time_test test passed' 
else
  echo '' 
  echo 'conffwk_time_test test failed'
  exit 1
fi

echo ''
echo "${1}/conffwk_dump -d oksconflibs:${test_data_file} -c Employee"
echo ''

if ${1}/conffwk_dump -d "oksconflibs:${test_data_file}" -c "Employee"
then
  echo ''
  echo 'conffwk_dump test passed'
else
  echo ''
  echo 'conffwk_dump test failed'
  exit 1
fi

echo '' 
echo '**********************************************************************'

###############################################################################

rm -f ${test_schema_file} ${test_data_file}

###############################################################################

schema_file="${2}/test/test.schema.xml"
data_file="/tmp/test-conffwk-by-`whoami`.$$"

echo ''
echo ''
echo '**********************************************************************'
echo '************* conffwk_test_rw test using oksconflibs plug-in ************'
echo '**********************************************************************'
echo ''

echo "${1}/conffwk_test_rw -d ${data_file} -s ${schema_file} -p oksconflibs"
echo ''

if ${1}/conffwk_test_rw -d ${data_file} -s ${schema_file} -p oksconflibs
then
  echo '' 
  echo 'conffwk_test_rw test passed' 
else
  echo '' 
  echo 'conffwk_test_rw test failed'
  exit 1
fi

rm -rf ${data_file}*

echo '' 
echo '**********************************************************************'

echo ''
echo ''
echo '**********************************************************************'
echo '************* conffwk_test_rw test using rdbconffwk plug-in ************'
echo '**********************************************************************'
echo ''

ipc_file="/tmp/ipc_root.$$.ref"
TDAQ_IPC_INIT_REF="file:${ipc_file}"
export TDAQ_IPC_INIT_REF

write_server_name='conffwk_w'
read_server_name='conffwk_r'
cleanup='ipc_rm -i ".*" -n ".*" -f'
startup_timeout='10'

echo 'destroy running ipc programs ...'
echo "${cleanup}"
${cleanup} > /dev/null 2>&1 || exit 1

trap 'echo "caught signal: destroy running ipc programs ..."; ipc_rm -i ".*" -n ".*"; exit 1' 1 2 15


  # run general ipc_server

echo 'ipc_server &'

ipc_server > /dev/null 2>&1 &
sleep 1 # is used to avoid waiting message

count=0
result=1
while [ $count -lt ${startup_timeout} ] ; do
  test_ipc_server
  result=$?
  if [ $result -eq 0 ] ; then break ; fi
  echo ' * waiting general ipc_server startup ...'
  sleep 1
  count=`expr $count + 1`
done

if [ ! $result -eq 0 ] ; then
  echo "ERROR: failed to run general ipc_server (timeout after $startup_timeout seconds)"
#  exit 1
fi


  # run rdb servers

echo "rdb_writer -d ${write_server_name} &"
rdb_writer -d ${write_server_name} > /dev/null 2>&1 &

echo "rdb_server -d ${read_server_name} -D daq/segments/setup-initial.data.xml &"
rdb_server -d ${read_server_name} -D daq/segments/setup-initial.data.xml > /dev/null 2>&1 &

sleep 1 # is used to avoid waiting message

count=0
result=1
while [ $count -lt ${startup_timeout} ] ; do
  test_corba_server -c 'rdb/writer' -n ${write_server_name}
  result=$?
  if [ $result -eq 0 ] ; then break ; fi
  echo ' * waiting rdb_writer startup ...'
  sleep 1
  count=`expr $count + 1`
done

if [ ! $result -eq 0 ] ; then
  echo "ERROR: failed to run rdb_writer (timeout after $startup_timeout seconds)"
fi


count=0
result=1
while [ $count -lt ${startup_timeout} ] ; do
  test_corba_server -c 'rdb/cursor' -n ${read_server_name}
  result=$?
  if [ $result -eq 0 ] ; then break ; fi
  echo ' * waiting rdb_server startup ...'
  sleep 1
  count=`expr $count + 1`
done

if [ ! $result -eq 0 ] ; then
  echo "ERROR: failed to run rdb_server (timeout after $startup_timeout seconds)"
fi

echo ""
echo ''

echo "${1}/conffwk_test_rw -d ${data_file} -s ${schema_file} -p rdbconffwk:${write_server_name}"
echo ''

if ${1}/conffwk_test_rw -d ${data_file} -s ${schema_file} -p "rdbconffwk:${write_server_name}"
then
  echo '' 
  echo 'conffwk_test_rw test passed' 
else
  echo '' 
  echo 'conffwk_test_rw test failed'
  exit 1
fi

echo '' 
echo ''
echo ''
echo '**********************************************************************'
echo '************ conffwk_dump java test using rdbconffwk plug-in ***********'
echo '**********************************************************************'
echo ''

echo $TDAQ_JAVA_HOME/bin/java -classpath ${1}/conffwk.jar:'$CLASSPATH' conffwk.ConfigDump -d "rdbconffwk:${read_server_name}" -C OnlineSegment -O -r
if $TDAQ_JAVA_HOME/bin/java -classpath ${1}/conffwk.jar:$CLASSPATH conffwk.ConfigDump -d "rdbconffwk:${read_server_name}" -C OnlineSegment -O -r
then
  echo '' 
  echo 'java conffwk_dump test passed' 
else
  echo '' 
  echo 'java conffwk_dump test failed'
  exit 1
fi

echo '' 
echo 'destroy running servers ...'

echo "rdb_admin -d ${write_server_name} -s"
rdb_admin -d ${write_server_name} -s

echo "rdb_admin -d ${read_server_name} -s"
rdb_admin -d ${read_server_name} -s

echo '' 
echo 'final cleanup...'

${cleanup} > /dev/null 2>&1
rm -rf ${data_file}* ${ipc_file}

echo '' 
echo '**********************************************************************'

