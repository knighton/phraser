Phraser is a DSL for recognizing English phrases.  It finds consecutive lists of subsequences that are defined by lists of tokens with embedded token-matching expressions.  Expressions consist of a type, arguments, and attribute filters.

Contents:
* [Demo](#demo)
* [Expressions](#expressions)
* [Expression syntax](#expression-syntax)
* [Phrase file syntax](#phrase-file-syntax)
* [Architecture](#architecture)
* [Tokenization](#tokenization)
* [Tagging](#tagging)

#### Demo

This phrase file:

    threat = subject, aux verb, intensifier, verb, object
    ----------
    (perspro +subj +3rd +thing)
    (DT) borg
    ----------
    will
    ----------
    
    fucking
    ----------
    assimilate
    ----------
    (posdet +thing) (butt|ass)
    (perspro +obj)
    
Plus this input text:

    The Borg will assimilate your ass.
    
Results in:

    TODO

#### Expressions

TODO

#### Expression syntax

    (<type> <0+ whitespace-separated args> <0+ whitespace-separated filters>)
    
or

    (<upper case Penn POS tag>)

or

    (<2+ args separated by '|'>)

where
* `(<upper case Penn POS tag>)` will be normalized to `(tag <upper case Penn POS tag>)`
* `(<2+ args separated by '|'>)` will be normalized to `(oneof <2+ args separated by '|'>)`
* an arg is arbitrary text not containing whitespace with `+`, `(`, and `)` escaped with `\`
* a filter is `^\+[a-z0-9]+$` (note the `+` prefix)

#### Phrase file syntax

    <phrase name> = <1+ comma-separated subsequence names>
    <1+ newline-separated sequences>

a subsequence is

    <dash divider>
    <1+ newline-separated item lists>

where
* a phrase name is `^[a-z ]+$`
* a subsequence name is `^[a-z ]+$`
* subsequence names will be trimmed on both sides
* the number of subsequence names must match the number of sequences
* a dash divider is `^\\-+$`
* an item list is 0+ space-separated items (ie, lines can be blank)
* an item is either a token or an expression
* a token is a string separable by whitespace
* an expression is a string containing arbitrary text separated by `(` and `)`
* occurences of `(` and `)` inside an expression must be escaped by `\`

#### Architecture

            Analyzer 
              | | \
              | |  Preprocessor (cc/preprocess/)
              | |        \
              | |         +--Destutterer (cc/preprocess/)
              |  \
             /   Tokenizer (cc/tokenization/)
            /
      PhraseDetector (cc/phrase/)
         /    \
        /   EnglishExpressionEvaluator (cc/expression/)
       /               \
    VectorMembership    +--PrecomputableEvaluators
    SequenceDetector    +--DynamicEvaluators
    (cc/sequence/)      +--AllAtOnceEvaluators (eg, Tagger)
                        (cc/english/, cc/tagging/)
    
    PreprocessStep
    * Destutterer
    
    SequenceDetector
    * EqualitySequenceDetector
    * VectorMembershipSequenceDetector
    
    ExpressionTypeEvaluator
    * PrecomputableEvaluator
    * DynamicEvaluator
    * AllAtOnceEvaluator

#### Tokenization

##### Overview

1. HTML entities (dec, hex, and named) are optionally converted to their Unicode code point equivalents.
2. Unicode is normalized to Penn Treebank ASCII like the Stanford parser.
3. The LAPOS tokenizer converts the ASCII text to ASCII tokens.
4. Tokens are normalized like the Stanford parser.

##### Lookup table generation:

You call a script [generate_tokenizer_data.py](https://github.com/knighton/phraser/blob/master/phraser/cc/tokenization/data_import/generate_tokenizer_data.py) that generates [tokenizer_data.h](https://github.com/knighton/phraser/blob/master/phraser/cc/tokenization/tokenizer_data.h), [tokenizer_data.cc](https://github.com/knighton/phraser/blob/master/phraser/cc/tokenization/tokenizer_data.cc) containing these tables:

* HTML named entity -> Unicode code point
* Block containing NULL-terminated normalized PTB ASCII strings
* Unicode code point -> index into that block
* ASCII token -> normalized ASCII token

##### Unicode to PTB ASCII normalization (preprocessing)

Essentially, we want to strip accents, map symbols to ASCII equivalents, and use LaTeX quotes.

The following steps occur for all Unicode code points in any index below in order to generate a static mapping:

1. Replace nonprintable ASCII with space (U+0020).
2. Normalize the various Unicode open/close quote styles to smart quotes (eg, `«` `»` to `“` `”`)
   * [quotes.txt](https://github.com/knighton/phraser/blob/master/phraser/cc/tokenization/data_import/quotes.txt)
3. Normalize currency symbols to `$` and `cents` (to match WSJ training data)
   * [currencies.txt](https://github.com/knighton/phraser/blob/master/phraser/cc/tokenization/data_import/currencies.txt)
4. Convert smart quotes to spaced Penn Treebank tokens (eg, `“` `”` to ``` `` ``` `''`)
   * [ptb_smart_quotes.txt](https://github.com/knighton/phraser/blob/master/phraser/cc/tokenization/data_import/ptb_smart_quotes.txt)
5. Decompose the Unicode code points according to NFKD
   * [nfc.txt](https://github.com/knighton/phraser/blob/master/phraser/cc/tokenization/data_import/nfc.txt) from ICU
   * [nfkc.txt](https://github.com/knighton/phraser/blob/master/phraser/cc/tokenization/data_import/nfkc.txt) from ICU
6. Replace non-ASCII Unicode code points with visually confusable code point sequences of type SA (same script, any case) that contain at least one ASCII code point
   * [confusables.txt](https://raw.githubusercontent.com/knighton/phraser/master/phraser/cc/tokenization/data_import/confusables.txt) from ICU
7. Filter out non-ASCII characters.
8. Join into a string.
9. Condense spaces.
10. Drop parenthesized non-Latin characters that don't map to ASCII (eg, U+3208 `㈈`).

##### Token normalization (postprocessing)

We make some changes in order to match the tagger's training data.

1. Certain punctuation tokens are escaped (eg, `(` to `-LRB-`)
   * [brackets.txt](https://github.com/knighton/phraser/blob/master/phraser/cc/tokenization/data_import/brackets.txt)
2. Commonwealth spellings are Americanized
   * [americanize.txt](https://github.com/knighton/phraser/blob/master/phraser/cc/tokenization/data_import/americanize.txt)

#### Tagging

Penn Treebank ASCII tokens (see [Tokenization](#tokenization)) are fed to the [LAPOS tagger](http://www.logos.ic.i.u-tokyo.ac.jp/~tsuruoka/lapos/) (vendorized [here](https://github.com/knighton/phraser/blob/master/phraser/cc/third_party/lapos/)), which uses a model pretrained on WSJ sections 2-21.
