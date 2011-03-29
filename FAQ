Does fIcy preserve the full quality of the stream?

	Yes, as explained in the README, there's no data loss.


Which audio formats fIcy supports?

	fIcy itself is not limited to any audio format, only to the transport
	used. For this reason fIcy can work transparently on MPEG1/Layer3,
	MPEG2/4 audio streams (usually called MP3/MP2/MP4 AAC). OGG is
	supported but, as titles are encoded in the OGG stream itself, track
	separation doesn't work. fResync only understands MPEG1 files.


Why not separate tracks on silence points?

	Short answer: nonsense.

	Long answer: silence detection would require full data decoding, would
	require reliable metaint positions and indeed would require real
	silence between tracks. When cross-fading/mixing is used, silence
	detection is just a waste of computing resource. On streams where
	silence between tracks *does* exist, metadata alone will do the work
	almost as well. Where metaint positions are useless, silence detection
	would be useless as well. We believe separation done that way is a lost
	war from the beginning.


Why not write MPEG tracks on frame boundaries directly?

	Maybe in a future release.


Can you insert id3/v2 tags in the files automagically?

	Several tools already exists for this. If you're really sure about
	adding tags, you may want to resync the files first as well.


How can I resync-and-tag files automatically as they're written?

	If you're really sure, you can do this by using the file produced by
	-q. A poor-man's solution could look like this::

	  $ fIcy -q foo ... &
	  $ tail -f foo | (while read file; do fResync "$file"; [your \
	    tagging command here]; done) &


Is there any plan for a Windows port?

	No. Don't ask for it.


How I can write to a single enormous file?

	Use the -o flag alone, without -m or -E::

	  fIcy -o file ...


How I can append to a single file?

	You can use the standard output instead of a named file::

	  fIcy ... >> file


fIcy aborts with "requested metadata, but got nothing"

	The stream runs without metadata (ie titles). No separation can take
	place. You can still write huge file chunks without the -mE flags.


How I can prevent fIcy to rip my station?

	Short answer: you can't.

	Long answer: if your streaming server supports this, you can block on
	the user-agent. fIcy uses "fIcy [version]" by default, and, contrarily
	to most other software, offers no way of changing it. Consider however
	that any user with a minimal amount of brain will be able to change it
	anyway. A more realistic solution is to disable metadata (ie titles) on
	your stream, but even then it won't prevent people for saving it on
	large chunks, and/or use ANY other software like winamp, xmms, wget,
	telnet, netcat, curl (or even your browser of choice).


Unanswered question?

	Check in the `mailing list archives`_, or subscribe_ to get support.
	Hopefully your question will appear here in the next release.

	.. _mailing list archives: http://news.gmane.org/gmane.comp.audio.ficy.user
	.. _subscribe: mailto:ficy-users+subscribe@thregr.org
