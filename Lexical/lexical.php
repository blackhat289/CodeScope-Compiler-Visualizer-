<?php
header('Content-Type: application/json');

$inputText = $_POST['indata'] . "\n";

$pipeConfig = array(
   0 => array("pipe", "r"),
   1 => array("pipe", "w"),
   2 => array("pipe", "w")
);

// Use absolute path - __DIR__ is the directory of this file
$binPath = __DIR__ . '/lexer';
$proc = proc_open($binPath, $pipeConfig, $streams);

if (is_resource($proc)) {

	fwrite($streams[0], $inputText);
	fclose($streams[0]);

	$output = stream_get_contents($streams[1]);
	fclose($streams[1]);

	if (!empty($output)) {
		echo "[" . $output . "]";
	} else {
		echo "[]";
	}

	proc_close($proc);
}
