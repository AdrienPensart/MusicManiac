# MusicManiac

## Description

It is a little GUI to manage a big, ordered, detailed discography. Such as Buckethead one.
You don't know who Buckethead is ? Just go to http://youtube.com and don't lose my time.

Some features : 

* Supports FLAC and MP3 metadatas : artist, comments, duration, UUID, rating
* Supports M3U playlists only
* List all music files and attach an UUID to each
* Generate playlist with saved parameters, and refresh playlist with new musics added :
  * One or many artists
  * WITH and WITHOUT keywords (attached to comment metadata)
  * a minimum and maximum duration
  * a minimum rating
* Force regeneration of all UUIDs
* Some playlist are specials : all.m3u and best.m3u
* Synchronization script with rsync to export musics listed in all.m3u or best.m3u and all *.m3u files

## How does it work ?

It scans a directory recursively and read the UUID of each music file supported, or generate one.
Then it reads playlists already generated by MusicManiac and refresh each playlist with musics added or deleted.

When you generate a new playlist, you chose many parameters which will be saved in each playlist.
Then each music is saved with its UUID, duration, path and name.

When a playlist is refreshed, the UUID is compared to the one in music metadatas. And you will be warned about missing files.

## Example of Playlist

### Playlist header

```
#EXTM3U
#EXTREM:musicmaniac
#EXTREM:artists:Buckethead
#EXTREM:rating:5
#EXTREM:minDuration:00:00
#EXTREM:maxDuration:100:00
#EXTREM:without:cutoff
#EXTREM:with:experimental
```

### Playlist entries

```
#EXTINF: 201,01 - Intro_ Park Theme.flac
#EXTREM:uuid c61eb844-e004-4ff4-a4e8-c456516f4cc7
Buckethead/1992 - Bucketheadland/CD1/01 - Intro_ Park Theme.flac
#EXTINF: 495,28 - Computer Master.flac
#EXTREM:uuid 99e0f23e-3fdb-4cb4-8d44-5de16e112b96
Buckethead/1992 - Bucketheadland/CD1/28 - Computer Master.flac
#EXTINF: 343,01 - Intro_ Park Theme Extension.flac
#EXTREM:uuid b9af5285-449b-4a1a-896d-f8670278caec
Buckethead/1992 - Bucketheadland/CD2/01 - Intro_ Park Theme Extension.flac
```

## ToDo

- rsync of best and all playlist to folder
- artist in playlist
