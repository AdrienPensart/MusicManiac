#!/usr/bin/env python

import taglib
import os
import fnmatch
import sys
import argparse
import vlc
import time
import re
import random
import urwid
from collections import defaultdict
from PlexPlaylistImporter import PlexPlaylistImporter

def find_files(directory, pattern):
    for root, dirs, files in os.walk(directory):
        for basename in files:
            if basename.endswith(tuple(pattern)):
                filename = os.path.join(root, basename)
                yield taglib.File(filename)

def restricted_float(x,y=0.0,z=5.0):
    x = float(x)
    if x < y or x > z:
        raise argparse.ArgumentTypeError("%r not in range [%r, %r]"%(x,y,z))
    return x

def get_artist(f, default=''):
    if 'ARTIST' not in f.tags:
        return default
    for item in f.tags['ARTIST']:
        return str(item)
    return default

def get_rating(f, default=0.0):
    if 'FMPS_RATING' not in f.tags:
        return default
    for item in f.tags['FMPS_RATING']:
        return float(item)
    return default

def get_description(f, default=''):
    if 'DESCRIPTION' not in f.tags:
        return default
    for item in f.tags['DESCRIPTION']:
        return str(item)
    return default

def get_keywords(f):
    return get_description(f).split()

def duration_to_seconds(duration):
    if re.match("\ds$", duration):
        return int(duration[:-1])
    if re.match("\dm$", duration):
        return int(duration[:-1])*60
    if re.match("\dh$", duration):
        return int(duration[:-1])*3600
    return duration

parser = argparse.ArgumentParser(
    prog="BucketManiac",
    description="Fast playlist generator and player",
    epilog="You'll never return iTunes")
parser.add_argument('folder', help='select folder to scan')
parser.add_argument('--artist', '-a', type=str, default='', help='filter by artist')
parser.add_argument('--rating', '-r', type=restricted_float, default=0.0,
    help='select minimum rating (between 0 and 5)')
parser.add_argument('--min_duration', '--min', type=str, default=None,
    help='filter with minimum duration (xx:xx:xx)')
parser.add_argument('--max_duration', '--max', type=str, default=None,
    help='filter with maximum duration (xx:xx:xx)')
parser.add_argument('--plexdb', type=str, default=None, help='import playlists to Plex (need root access)')
# sudo chmod og+w "/var/lib/plexmediaserver/Library/Application Support/Plex Media Server/Plug-in Support/Databases/com.plexapp.plugins.library.db"
# sudo chmod og+rw "/var/lib/plexmediaserver/Library/Application Support/Plex Media Server/Plug-in Support/Databases/com.plexapp.plugins.library.db-shm"
# sudo chmod og+rw "/var/lib/plexmediaserver/Library/Application Support/Plex Media Server/Plug-in Support/Databases/com.plexapp.plugins.library.db-wal"
parser.add_argument('--noflac', action='store_true', help='disable flac loading')
parser.add_argument('--nomp3', action='store_true', help='disable mp3 loading')
parser.add_argument('--genm3u', action='store_true', help='autogenerate best playlists by keywords')
parser.add_argument('--withtags', type=str, default=None, help='filter by tags')
parser.add_argument('--withouttags', type=str, default=None, help="don't select files containing tags")
parser.add_argument('--show', action='store_true', help="show all tags")
parser.add_argument('--play', '-p', action='store_true', help='play list of files')
parser.add_argument('--shuffle', '--random', '-s', action='store_true', help='shuffle playlist')
parser.add_argument('--loop', '-l', action='store_true', help='play files in loop')
parser.add_argument('--generate', '-g', type=str, default=None, help='generate m3u playlist')
parser.add_argument('--relative', action='store_true', help='generate playlist with relative path')
parser.add_argument('--gui', action='store_true', help='start GUI')
parser.add_argument('--version', '-v', action='version', version='%(prog)s 1.0')
args = parser.parse_args()

# hide tags error in files, by default taglib print them on stderr
#os.close(sys.stderr.fileno())

formats = list()
if not args.noflac:
    formats.append('flac')
if not args.nomp3:
    formats.append('mp3')
files = find_files(args.folder, formats)
musics = list()
rating = args.rating / 5.0

if args.plexdb is not None:
    args.relative = True

try:
    if args.min_duration is not None:
        minduration = duration_to_seconds(args.min_duration)
    if args.max_duration is not None:
        maxduration = duration_to_seconds(args.max_duration)
except ValueError as err:
    print('Bad format',err)
    sys.exit(0)

if args.withtags is not None:
    withtags = args.withtags.split(',')
if args.withouttags is not None:
    withouttags = args.withouttags.split(',')

for f in files:
    if args.min_duration is not None and f.length < minduration:
        continue
    if args.max_duration is not None and f.length > maxduration:
        continue
    if get_rating(f) < rating:
        continue
    if args.artist != '' and args.artist != f.tags['ARTIST']:
        continue
    if args.withouttags is not None:
        found = False
        for mtag in withouttags:
            if mtag in get_keywords(f):
                 found = True
                 break
        if found:
            continue
    if args.withtags is not None:
        notfound = False
        for mtag in withtags:
            if mtag not in get_keywords(f):
                notfound = True
        if notfound:
            continue
    musics.append(f)
    if args.show:
        print(f, f.tags)
    f.close()

if args.shuffle:
    random.shuffle(musics)

artists = defaultdict(list)
playlists = defaultdict()
if args.genm3u:
    for m in musics:
        artists[get_artist(m)].append(m)
    for artist, musics in artists.items():
        beginning = os.path.join(args.folder,artist+'/')
        filename = os.path.join(args.folder,artist,'best.m3u')
        m3u = open(filename, 'w')
        playlists[filename] = artist + " Best"
        m3u.write("#EXTM3U\n")
        files = defaultdict()
        for m in musics:
            if args.relative:
                if m.path.startswith(beginning):
                    m3u.write(m.path[len(beginning):]+'\n')
                else:
                    print("Invalid beginning for relative path", m.path, "beginning is", beginning)
                    continue
            else:
                m3u.write(m.path+'\n')
            for keyword in get_keywords(m):
                if keyword not in files:
                    filename = os.path.join(args.folder,artist,keyword+'.m3u')
                    files[keyword] = open(filename, 'w')
                    playlists[filename] = artist + " " + keyword.capitalize()
                    files[keyword].write("#EXTM3U\n")
                if args.relative:
                    if m.path.startswith(beginning):
                        files[keyword].write(m.path[len(beginning):]+'\n')
                    else:
                        print("Invalid beginning for relative path", m.path, "beginning is", beginning)
                else:
                    files[keyword].write(m.path+'\n')
        for k,f in files.items():
            f.close()
        m3u.close()

if args.plexdb is not None:
    print('using DB',args.plexdb)
    for path,name in playlists.items():
        PlexPlaylistImporter.ImportIntoPlex(path, name, args.plexdb)

if args.generate is not None:
    filename = os.path.join(args.folder, args.generate)
    m3u = open(filename, 'w')
    m3u.write("#EXTM3U\n")
    for m in musics:
        m3u.write(m.path+'\n')
    m3u.close()

if args.play:
    vlc_instance = vlc.Instance()
    while True:
        for m in musics:
            media = vlc_instance.media_new_path(m.path)
            player = vlc_instance.media_player_new()
            player.set_media(media)
            player.play()
            print ('Playing', m.path, 'of', m.length, 'seconds')
            time.sleep(m.length)
            player.stop()
        if not args.loop:
            break

if args.gui:
    txt = urwid.Text(u"Hello World")
    fill = urwid.Filler(txt, 'top')
    loop = urwid.MainLoop(fill)
    loop.run()
