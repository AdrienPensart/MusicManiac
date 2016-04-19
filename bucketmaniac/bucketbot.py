#!/usr/bin/env python

## PLEX
## sudo chmod og+w "/var/lib/plexmediaserver/Library/Application Support/Plex Media Server/Plug-in Support/Databases/com.plexapp.plugins.library.db*"
#
"""BucketBot.
Description:
  Scan folders and list music tags for mp3 and flac.
  Can play those files with filters
  Generate your playlists easily

Usage:
  bucketbot.py tags <folder> [--artists=<artists>] [--min-duration=<duration>] [--max-duration=<duration>] [--with=<keywords>] [--without=<keywords>] [--rating=<rating>] [--fields=<fields>] [--quiet] [--mp3] [--flac] [--verbose]
  bucketbot.py play <folder> [--loop] [--shuffle | --random] [--artists=<artists>] [--min-duration=<duration>] [--max-duration=<duration>] [--with=<keywords>] [--without=<keywords>] [--rating=<rating>] [--fields=<fields>] [--quiet] [--mp3] [--flac]
  bucketbot.py playlist new <folder> <path> [--shuffle | --random] [--relative]
  bucketbot.py playlist bests <folder> [--shuffle | --random] [--relative]
  bucketbot.py plex import <folder> [--db=<path>]
  bucketbot.py (-h | --help)
  bucketbot.py (--version | -V)
  bucketbot.py --gui

Options:
  --artists=<artists>        Artist list filter, comma separated
  --min-duration=<duration>  Minimum duration filter (hours:minutes:seconds)
  --max-duration=<duration>  Maximum duration filter (hours:minutes:seconds)
  --with=<keywords>          Filter with those tags, comma separated
  --without=<keywords>       Filter without those tags, comma separeted
  --rating=<rating>          Minimum rating (between 0.0 and 5.0) [default: 0.0]
  --fields=<fields>          Show only those fields (artist, title, etc), comma separated
  --help -h                  Show this screen
  --quiet                    Hide error stream [default: False]
  --verbose                  Increase verbosity level [default: False]
  --version                  Show version
  --mp3                      Scan only MP3 files [default: False]
  --flac                     Scan only FLAC files [default: False]
  --shuffle --random         Randomize selection [default: False]
  --loop                     Play files in loop [default: False]
  --gui                      Start GUI [default: False]
  --db=<db>                  Path to Plex database [default: /var/lib/plexmediaserver/Library/Application Support/Plex Media Server/Plug-in Support/Databases/com.plexapp.plugins.library.db]
"""
from docopt import docopt
import taglib
import os
import fnmatch
import sys
import vlc
import time
import re
import random
import urwid
from collections import defaultdict
from PlexPlaylistImporter import PlexPlaylistImporter
import plexapi

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

def get_first(f, tag, default=''):
    if tag not in f.tags:
        return default
    for item in f.tags[tag]:
        return str(item)
    return default

def get_artist(f, default=''):
    return get_first(f, 'ARTIST', default)

def get_rating(f, default='0.0'):
    return get_first(f, 'FMPS_RATING', default)

def get_description(f, default=''):
    return get_first(f, 'DESCRIPTION', default)

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

args = docopt(__doc__, version='BucketBot 1.0')

# hide tags error in files, by default taglib print them on stderr
if args['--quiet']:
    os.close(sys.stderr.fileno())

vprint = print if args['--verbose'] else lambda *a, **k: None

formats = ['flac','mp3']
if args['--flac']:
    formats = ['flac']
if args['--mp3']:
    formats = ['mp3']
files = list()
if args['<folder>']:
	files = find_files(args['<folder>'], formats)
musics = list()
rating = float(args['--rating']) / 5.0

if args['plex'] is True:
    vprint("Plex DB needs relative paths")
    args['--relative'] = True

try:
    if args['--min-duration'] is not None:
        minduration = duration_to_seconds(args['--min-duration'])
    if args['--max-duration'] is not None:
        maxduration = duration_to_seconds(args['--max-duration'])
except ValueError as err:
    print('Bad format',err)
    sys.exit(0)

withtags = list()
if args['--with'] is not None:
    withtags = args['--with'].split(',')
withouttags = list()
if args['--without'] is not None:
    withouttags = args['--without'].split(',')
artistsfilter = list()
if args['--artists'] is not None:
	artistsfilter = args['--artist'].split(',')

for f in files:
    vprint('Analyzing',f.path)
    if args['--min-duration'] is not None and f.length < minduration:
        vprint('Bad min duration', f.length, minduration)
        continue
    if args['--max-duration'] is not None and f.length > maxduration:
        vprint('Bad max duration', f.length, maxduration)
        continue
    if float(get_rating(f)) < rating:
        vprint('Bad rating', get_rating(f), rating)
        continue
    if args['--artists'] is not None:
        martist = get_artist(f)
        for artist in artistsfilter:
            if artist == martist:
                vprint('Bad artist', artist, get_artist(f))
                continue
    if args['--without'] is not None:
        found = False
        for mtag in withouttags:
            if mtag in get_keywords(f):
                 found = True
                 break
        if found:
            vprint('Bad keyword', withouttags, get_keywords(f))
            continue
    if args['--with'] is not None:
        notfound = False
        for mtag in withtags:
            if mtag not in get_keywords(f):
                notfound = True
        if notfound:
            vprint('Mandatory tag not found', withtags, get_keywords(f))
            continue
    musics.append(f)
    vprint('Appending',f.path)
    if args['tags'] is True:
        if args['--fields'] is not None:
            print([get_first(f, field.upper()) for field in args['--fields'].split(',')])
        else:
            print(f.path, f.tags, f.unsupported)

if args['--shuffle'] is True:
    random.shuffle(musics)

artists = defaultdict(list)
playlists = defaultdict()
if args['playlist'] is True and args['bests'] is True:
    for m in musics:
        artists[get_artist(m)].append(m)
    for artist, musics in artists.items():
        beginning = os.path.join(args['<folder>'],artist+'/')
        filename = os.path.join(args['<folder>'],artist,'best.m3u')
        m3u = open(filename, 'w')
        playlists[filename] = artist + " Best"
        m3u.write("#EXTM3U\n")
        files = defaultdict()
        for m in musics:
            if args['--relative']:
                if m.path.startswith(beginning):
                    m3u.write(m.path[len(beginning):]+'\n')
                else:
                    print("Invalid beginning for relative path", m.path, "beginning is", beginning)
                    continue
            else:
                m3u.write(m.path+'\n')
            for keyword in get_keywords(m):
                if keyword not in files:
                    filename = os.path.join(args['<folder>'],artist,keyword+'.m3u')
                    files[keyword] = open(filename, 'w')
                    playlists[filename] = artist + " " + keyword.capitalize()
                    files[keyword].write("#EXTM3U\n")
                if args['--relative']:
                    if m.path.startswith(beginning):
                        files[keyword].write(m.path[len(beginning):]+'\n')
                    else:
                        print("Invalid beginning for relative path", m.path, "beginning is", beginning)
                else:
                    files[keyword].write(m.path+'\n')
        for k,f in files.items():
            f.close()
        m3u.close()

if args['plex'] is True:
    for path,name in playlists.items():
        PlexPlaylistImporter.ImportIntoPlex(path, name, args['--db'])

if args['playlist'] and args['new']:
    m3u = open(args['<path>'], 'w')
    m3u.write("#EXTM3U\n")
    for m in musics:
        m3u.write(m.path+'\n')
    m3u.close()

if args['play']:
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
        if not args['--loop']:
            break

if args['--gui']:
    txt = urwid.Text(u"Hello World")
    fill = urwid.Filler(txt, 'top')
    loop = urwid.MainLoop(fill)
    loop.run()
