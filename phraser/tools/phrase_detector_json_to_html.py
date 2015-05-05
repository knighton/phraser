#!/usr/bin/python
#
# Dump a JSON dump of a PhraseDetector to HTML.


HEAD = """
<html>
<head>
<style type="text/css">
body {
    background: #a0c0e0;
}
table {
    width: 100%;
}
table, tr, td {
    border-spacing: 0px;
}

/* phrases */
.phrase {
    margin: 10px;
    padding: 10px;
    border-radius: 5px;
    background: #b0d0f0;
}
.token_groups {
    padding: 10px;
    padding-left: 5px;
    padding-right: 5px;
    margin: 5px;
    border-radius: 5px;
    background: #b8d8f8;
}
.token_group {
    padding: 5px;
    border-radius: 5px;
    background: white;
}
.divider {
    color: #ccc;
    font-weight: bold;
}

/* expression evaluators */
.expression_evaluator {
    margin: 5px;
    padding: 10px;
    border-radius: 5px;
    background: #b0d0f0;
    background: #b0d0f0;
}
.left_td {
    width: 150px;
}
.dim_values {
    margin: 0px;
    padding: 10px;
    padding-left: 5px;
    padding-right: 5px;
    border-radius: 5px;
    background: #b8d8f8;
}
.dim_value {
    padding: 5px;
    border-radius: 5px;
    background: white;
}
</style>
</head>
<body>
"""[1:]


FOOT = """
</body>
</html>
"""[1:]


def parse_args():
    from argparse import ArgumentParser
    ap = ArgumentParser()
    ap.add_argument('--json', type=str, required=True)
    return ap.parse_args()


def dump_phrase(j, expressions, tokens):
    print '<div class="phrase">'
    print j['name']
    print '<ul>'
    for block_index, block in enumerate(j['blocks']):
        print '<li>'
        print block_index, j['block_names'][block_index]
        print '<ul>'
        for option_index, option in enumerate(block):
            print '<li>'
            print '<div class="token_groups">'
            print option_index
            for group_id_index, group_id in enumerate(option):
                ss = []
                ss.append('<b>%d</b>' % group_id)
                ss.append('<span class="divider">||</span>')
                if group_id % 2:
                    ss.append(tokens[group_id / 2])
                else:
                    ss.append(expressions[group_id / 2])
                print '<span class="token_group">%s</span>' % ' '.join(ss)
            print '</div>'
            print '</li>'
        print '</ul>'
        print '</li>'
    print '</ul>'
    print '</div>'


def dump_vocab(j):
    print '<div class="vocab">'
    print '<ul>'
    for behavior in ['all_at_once_evaluators', 'dynamic_evaluators',
                     'precomputable_evaluators']:
        print '<li>'
        print behavior
        print '<ul>'
        for etype in sorted(j[behavior]):
            print '<li>'
            print '<div class="expression_evaluator">'
            print '<b>%s</b>' % etype
            print '<br>'
            print '<br>'
            print '<table>'
            d = j[behavior][etype]['dimension2possible_values']
            for dim in sorted(d):
                values = sorted(d[dim])
                values = map(lambda s: '<span class="dim_value">%s</span>' % s,
                             values)
                values_s = ' '.join(values)

                left = '<td class="left_td">%s</td>' % dim
                right = '<td><div class="dim_values">%s</div></td>' % values_s
                print '<tr>%s%s</tr>' % (left, right)
            print '</table>'
            print '</div>'
            print '</li>'
        print '</ul>'
        print '</li>'
    print '</ul>'
    print '</div>'


def run(args):
    import json
    s = open(args.json).read()
    j = json.loads(s)

    expressions = j['vocab']['vocabulary']['expressions']
    tokens = j['vocab']['vocabulary']['tokens']

    print HEAD

    print '<ul>'

    print '<li>'
    print 'phrase detectors'
    for sub_j in j['phrases']:
        dump_phrase(sub_j, expressions, tokens)
    print '</li>'

    print '<li>'
    print 'expression evaluators'
    dump_vocab(j['vocab'])
    print '</li>'

    print '</ul>'

    print FOOT


if __name__ == '__main__':
    run(parse_args())
