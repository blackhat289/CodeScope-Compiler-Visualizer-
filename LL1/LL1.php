<?php
header('Content-Type: application/json');

$inputPayload = $_POST['data'] . "\n#\n#";

$pipeConfig = array(
   0 => array("pipe", "r"),
   1 => array("pipe", "w"),
);

// Use absolute path - __DIR__ is the directory of this file
$binary = __DIR__ . '/grammar_parser';

$processHandle = proc_open($binary, $pipeConfig, $streams);

if (is_resource($processHandle)) {

	fwrite($streams[0], $inputPayload);
	fclose($streams[0]);

	$result = stream_get_contents($streams[1]);
	fclose($streams[1]);

	echo $result;

	proc_close($processHandle);
}
