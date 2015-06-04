#!/usr/bin/python
#
# Gzipped files -> one comment per line, with tags.
#
# Also dumps statistics about what it processed for sanity checking.
#
# Takes about 20 minutes to extract all of Impermium.

from argparse import ArgumentParser
from collections import defaultdict
from glob import glob
import gzip
import json
import os
import time


def parse_args():
    ap = ArgumentParser()
    ap.add_argument('--in_glob', type=str, required=True)
    ap.add_argument('--out_prefix', type=str, default='comments_')
    return ap.parse_args()


def each_json_from_file(file_name):
    with gzip.open(file_name) as f:
        for line in f:
            j = json.loads(line)
            yield j


def date_from_file_name(s):
    s = os.path.basename(s)
    return s[:s.index('.')]


def tags_from_log(j):
    try:
        return j['magicmod']['result']['tags']
    except:
        pass

    try:
        return j['impermium']['result']['tags']
    except:
        pass

    try:
        return j['impermium']['result']['4.0']['tags']
    except:
        pass

    return None


def main(args):
    t0 = time.time()
    f2key2stuff = defaultdict(dict)
    threat_tags = ['mild_threat', 'strong_threat', 'threat']
    for file_name in sorted(glob(args.in_glob)):
        no_content = 0
        date = date_from_file_name(file_name)
        fn = '%s%s.txt' % (args.out_prefix, date)
        if os.path.exists(fn):
            print '%s exists, skipping...' % fn
            continue
        f = open(fn, 'wb')
        no_tags_field = 0
        ok = 0
        tag2count = defaultdict(int)
        for j in each_json_from_file(file_name):
            # Handle tags.
            tags = tags_from_log(j)
            if tags is None:
                no_tags_field += 1
                tags = []
            else:
                for tag in set(tags):
                    tag2count[tag] += 1

            tags = map(lambda tag: tag.encode('utf-8'), tags)
            num_tags = str(len(tags))
            try:
                text = j['object']['content']
            except:
                no_content += 1
                continue
            text = ' '.join(text.split()).encode('utf-8')

            line = ' '.join([num_tags] + tags + [text])
            f.write('%s\n' % line)
            ok += 1

        # Sanity checks.
        if ok:
            assert no_tags_field < ok
        assert no_content < 5

        f2key2stuff[file_name]['ok'] = ok
        f2key2stuff[file_name]['no_tags_field'] = no_tags_field
        f2key2stuff[file_name]['tag2count'] = tag2count
        t1 = time.time()
        print json.dumps({
            'time': t1 - t0,
            'file_name': file_name,
            'f2key2stuff': f2key2stuff[file_name],
            'no_content': no_content,
        })
        f.close()
    print json.dumps(f2key2stuff, indent=4)


if __name__ == '__main__':
    main(parse_args())
