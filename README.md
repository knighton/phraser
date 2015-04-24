## phraser

Phraser is a DSL for recognizing English phrases.  It finds consecutive lists of subsequences using an expression language that matches tokens.

Expressions consist of a type, arguments, and attribute filters.  Types include:
* part-of-speech tag
* special categories of English words
* user-defined groups of words
* regex
* etc.

Contents:
* [Demo](#demo)
* [Expressions](#expressions)
* [Expression syntax](#expression-syntax)
* [Phrase file syntax](#phrase-file-syntax)
* [Architecture](#architecture)

iamknighton@gmail.com

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
    <1+ item lists>

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
