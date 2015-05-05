#!/usr/bin/python
#
# This is step 2 of 2.  Run like this:
#
#     # 1. Find matches in all the comments -- takes a few minutes.
#     ./build/compare_against_impermium /media/x/impermium_dedup/comment_*txt
#
#     # 2. Dudupe and format the output as pretty HTML.
#     ./tools/compare_against_impermium_postprocess.py \
#         james_only.txt both.txt impermium_only.txt
#
# Expects lines that look like
#
#     "2 mild_threat strong_profanity 4 3 12 9 15 This is a comment blah blah"
#
# which would mean
#
#     tags: mild_threat, strong_profanity
#     matches at: [3, 12), [9, 15)
#     comment text with whitespace cleaned up: "This is a comment blah blah"

from collections import defaultdict
import sys


HEAD = """
<html>
<head>
    <style type="text/css">
body { background: #ace; }
.comment { background: #bdf; margin: 5px; padding: 5px; }
.text { margin: 5px; margin-bottom: 9px; padding: 4px; border-radius: 4px; background: #c4e4ff; }
.match { background: #fe0; padding-top: 4px; padding-bottom: 4px; border-radius: 4px; }
.tags { margin: 5px; }
.tag { background: #fca; padding: 4px; border-radius: 4px; }
    </style>
</head>
<body>
"""[1:]


FOOT = """
</body>
</html>
"""[1:]


def each_line(file_name):
    with open(file_name) as f:
        for line in f:
            if not line or line.isspace():
                continue
            yield line


def comment_from_line(line):
    ss = line.split()
    num_tags = int(ss[0])
    tags = ss[1:1 + num_tags]

    x = 1 + num_tags
    num_indexes = int(ss[x])
    indexes = map(int, ss[x + 1:x + 1 + num_indexes])

    x += 1 + num_indexes
    codes = map(lambda s: int(s, 16), ss[x:])
    text = ''.join(map(unichr, codes))

    match_spans = []
    assert not len(indexes) % 2
    for i in xrange(len(indexes) / 2):
        match_spans.append((indexes[i * 2], indexes[i * 2 + 1]))

    match_spans = sorted(match_spans)

    # Technically, they can be overlapping.  But in practice, they aren't
    # Let's pretend they can't be so we can show all matches in a single row.
    for i in xrange(1, len(match_spans)):
        try:
            assert match_spans[i - 1][1] < match_spans[i][0]
        except:
            print 'OOPS'
            match_spans = []
            break

    return (text, tuple(tags), tuple(match_spans))


def html_escape(s):
    ss = []
    for c in s:
        n = ord(c)
        if 0x20 <= n <= 0x7E:
            ss.append(c)
        else:
            ss.append('&#x%x;' % n)
    return ''.join(ss)


def highlight_text(text, spans):
    ss = []
    prev_end_excl = 0
    for begin, end_excl in spans:
        ss.append(text[prev_end_excl : begin])
        ss.append('<span class="match">')
        ss.append(text[begin : end_excl])
        ss.append('</span>')
        prev_end_excl = end_excl
    ss.append(text[prev_end_excl : ])
    text_s = ''.join(map(html_escape, ss))
    return '<div class="text">%s</div>' % text_s


def show_tags(tags):
    tags_ss = map(lambda tag: '<span class="tag">%s</span>' % tag, tags)
    return '<div class="tags">%s</div>' % ' '.join(tags_ss)


def make_html(file_name):
    text2set = defaultdict(set)
    for line in each_line(file_name):
        c = comment_from_line(line)
        text = c[0]
        text2set[text].add(c)

    # Verify that phraser results are deterministic.
    for text, cc_set in text2set.iteritems():
        try:
            assert len(cc_set) == 1
        except:
            print 'WTF'
            print cc_set

    lines = []
    for text in sorted(text2set):
        for _, tags, spans in text2set[text]:
            text_s = highlight_text(text, spans)
            tags_s = show_tags(tags)
            line = '<div class="comment">%s%s</div>\n' % (text_s, tags_s)
            lines.append(line)

    with open(file_name[:-4] + '.html', 'wb') as f:
        f.write(HEAD)
        for line in lines:
            f.write(line)
        f.write(FOOT)

    return True


def run(ff):
    for f in ff:
        assert f.endswith('.txt')
        assert make_html(f)


if __name__ == '__main__':
    run(sys.argv[1:])
