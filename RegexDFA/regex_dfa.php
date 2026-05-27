<?php
header('Content-Type: application/json');

$regex = $_POST['regex'];
$teststr = isset($_POST['teststr']) ? $_POST['teststr'] : '';

$inputPayload = $regex . "\n#\n" . $teststr . "\n";

$pipeConfig = array(
   0 => array("pipe", "r"),
   1 => array("pipe", "w"),
);

$binary = __DIR__ . '/regex_dfa';

$processHandle = proc_open($binary, $pipeConfig, $streams);

if (is_resource($processHandle)) {

	fwrite($streams[0], $inputPayload);
	fclose($streams[0]);

	$result = stream_get_contents($streams[1]);
	fclose($streams[1]);

	echo $result;

	proc_close($processHandle);
}
