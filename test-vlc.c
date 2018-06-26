/*
 * test-vlc.c
 *
 * Copyright 2018 alexmik <alexmik@alexmik_archlinux>
 *
 */


/*
 * Simple app to play an mp3 with open-source vlc library.
 *
 * Package needed: libvlccore-dev libvlc-dev
 *
 * To link and compile:
 * gcc $(pkg-config --cflags libvlc) -c vlc-test.c -o vlc-test.o
 * gcc vlc-test.o -o vlc-test $(pkg-config --libs libvlc)
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>	// required by sleep()
#include <sys/types.h>	//required by DIR
#include <dirent.h>	//same up
#include <errno.h>
#include <string.h>

#include <vlc/vlc.h>

// example of an event manager request
void my_callback(const libvlc_event_t *event, void *param) {
	if(event->type == libvlc_MediaPlayerPlaying)
		printf("Play !\n");
	fflush(stdout);
	/*
	switch(event->type)
	{
		case libvlc_MediaPlayerPlaying:
			printf("Playing\n");
			break;
		case libvlc_MediaPlayerPaused:
			printf("Paused\n");
			break;
		case libvlc_MediaPlayerStopped:
			printf("Stopped\n");
			break;
		case libvlc_MediaPlayerEndReached:
			printf("End reached\n");
			break;
		case libvlc_MediaPlayerTimeChanged:
			printf("Time changed\n");
			break;
		case libvlc_MediaPlayerPositionChanged:
			printf("Player position changed\n");
			break;
		case libvlc_MediaPlayerLengthChanged:
			printf("Length changed\n");
			break;
		case libvlc_MediaPlayerSnapshotTaken:
			printf("Snapshot taken\n");
			break;
		case libvlc_MediaPlayerEncounteredError:
			printf("libvlc_MediaPlayerEncounteredError received,
			this is not looking good...\n");
			break;
		case libvlc_MediaPlayerSeekableChanged:
			break;
		case libvlc_MediaPlayerPausableChanged:
		case libvlc_MediaPlayerTitleChanged:
		case libvlc_MediaPlayerNothingSpecial:
		case libvlc_MediaPlayerOpening:
		case libvlc_MediaPlayerBuffering:
		case libvlc_MediaPlayerForward:
		case libvlc_MediaPlayerBackward:

		default:
			printf("Unknown mediaPlayerEvent: PAAANIC!\n");
	}
	*/
}

// Register the callback
void registerEvents(libvlc_event_manager_t *e_man){
	libvlc_event_attach( e_man, libvlc_MediaPlayerSnapshotTaken,   my_callback, NULL );
	libvlc_event_attach( e_man, libvlc_MediaPlayerTimeChanged,     my_callback, NULL );
	libvlc_event_attach( e_man, libvlc_MediaPlayerPlaying,         my_callback, NULL );
	libvlc_event_attach( e_man, libvlc_MediaPlayerPaused,          my_callback, NULL );
	libvlc_event_attach( e_man, libvlc_MediaPlayerStopped,         my_callback, NULL );
	libvlc_event_attach( e_man, libvlc_MediaPlayerEndReached,      my_callback, NULL );
	libvlc_event_attach( e_man, libvlc_MediaPlayerPositionChanged, my_callback, NULL );
	libvlc_event_attach( e_man, libvlc_MediaPlayerLengthChanged,   my_callback, NULL );
	libvlc_event_attach( e_man, libvlc_MediaPlayerEncounteredError,my_callback, NULL );
	libvlc_event_attach( e_man, libvlc_MediaPlayerPausableChanged, my_callback, NULL );
	libvlc_event_attach( e_man, libvlc_MediaPlayerSeekableChanged, my_callback, NULL );
}

void listDirFiles(){
	DIR *dir = NULL;
	struct dirent *dp;

	dir = opendir("/home/alexmik/Musica/Rock");

	while(dir){
		errno = 0;
		if((dp = readdir(dir)) != NULL){
			puts(dp->d_name);
		} else {
		closedir(dir);
		dir = NULL;
		}
	}
}

char* fileToPlay(){
	char *name = (char *)malloc(sizeof(char)*20);
	printf("Which file want to play?\n");
	listDirFiles();
	scanf("%s", name);
	char *res = strcat("/home/alexmik/Musica/Rock/", name);
	return res;
}
int main()
{
	libvlc_instance_t *inst;	//instance of vlc library
	libvlc_event_manager_t *e_man;	//instance of an event manager
	libvlc_media_player_t *mp;	//instance of media-player
	libvlc_media_t *m;		//media file to play

	// load the vlc engine
	inst = libvlc_new(0, NULL);

	if(inst == NULL) {
		printf("There was an error initializing VLC\n");
		exit(1);
	} else {
		printf("VLC initialized successfully\n");
	}

	// create a new item
	m = libvlc_media_new_path(inst, "path to MP3 file");

	// create a media-player playing environment
	mp = libvlc_media_player_new_from_media(m);

	// create an event manager for the media player's instance
	e_man = libvlc_media_player_event_manager(mp);

	// function to associate an event handler with any event
	// in this example it listen to media-player start playing
	libvlc_event_attach(e_man, libvlc_MediaPlayerPlaying, my_callback, NULL);
	//registerEvents(e_man);

	// no need to keep the media now
	libvlc_media_release(m);

	// to check song/movie length
	int length = libvlc_media_player_get_length(mp);

	if(length == -1)
		printf("there is no media!\n");
	else
		printf("song's length: %d", length);

	// play the media_player
	libvlc_media_player_play(mp);

	// check if media-player is playing
	printf("media-player is playing (1=Y | 0=N): %d\n", libvlc_media_player_is_playing(mp));

	// to seek song.
	// Set movie position as percentage between 0.0 and 1.0.
	// This has no effect if playback is not enabled.
	//libvlc_media_player_set_position(libvlc_media_player_t *p_mi, float f_pos)


	sleep(10);

	// toggle pause playing
	//libvlc_media_player_play(libvlc_media_player_t *p_mi)
	//0 if playback started (and was already started), or -1 on error.

	// stop playing
	libvlc_media_player_stop(mp);

	// free the media_player
	libvlc_media_player_release(mp);

	//free vlc library instance
	libvlc_release(inst);

	return 0;
}
