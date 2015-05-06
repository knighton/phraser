#!/usr/bin/python
import os
print os.getcwd()

import sys
sys.path.append('.')

from py.phraser_wrapper import analyze, init


def main():
    phrase_config_ff = [
        'config/threat_command.txt',
        'config/threat_statement.txt',
    ]
    
    assert init(phrase_config_ff)

    while True:
        s = raw_input('Comment: ')
        s = s.decode('utf-8')
        r = analyze(s)
        print
        print r.original_text
        print r.clean_text
        print r.tokens
        for match in r.phrase_matches:
            for indexes in match.index_lists:
                print indexes
        print


if __name__ == '__main__':
    main()
