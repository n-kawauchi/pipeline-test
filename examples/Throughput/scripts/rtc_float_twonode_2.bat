start .\ThroughputComp.exe -f rtc_float_twonode_2.conf
TIMEOUT /T 3
cmd /c rtcon /localhost/Throughput0_1.rtc:out /localhost/Throughput0_2.rtc:in

cmd /c rtact /localhost/Throughput0_1.rtc