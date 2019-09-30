# binsync

this is a tool which compares an input and output file in chunks of
1KiB and rewrites the chunks in the output file if they're different.

an example use case would be where you have a VM disk image, and a
copy of it, and after running the VM want to update the copy without
rewriting the entire file. This is more important if you have very
big files and write speeds are slow or you're using an SSD and don't
want to write unnecessarily.

