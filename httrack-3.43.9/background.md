This is a an order violation concurrency bug.

HTTrack is a offline browser utility. It allows you to 
download a World Wide Web site from the Internet to a
local directory.

In httrack, htsserver would invoke "webhttrack_runmain"
function to handle connectioin, and a global variable 
"global_opt" is created in the main loop (htsweb.c:262). 
The variable may be accessed by other thread, for instance,
"hts_cancel_file_push" function (htscore.c:3139) is called
by another thread to cancel a file.