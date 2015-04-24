## phraser

Phraser is a DSL for recognizing English phrases.  It finds consecutive lists of subsequences using an expression language that matches tokens.

Expressions consist of a type, arguments, and attribute filters.  Types include:
* part-of-speech tag
* special categories of English words
* user-defined groups of words
* regex
* etc.

Contents:
* Demo
* Phrase file syntax
* Expression syntax
* Available expressions
* Architecture

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

#### Phrase file syntax

    <phrase name> = <list of 1+ comma-separated subsequence names>
    <list of 1+ newline-separated sequences>

a sequence is

    <dash divider>
    <list of 1+ item lists>

where
* a phrase name is `^[a-z ]+$`
* a subsequence name is `^[a-z ]+$`
* the number of subsequence names must match the number of sequences
* a sequence is `<dash divider>\n<list of 1+ item lists>`
* a dash divider is `^\\-+$`
* an item list is a list of 0+ space-separated items (ie, lines can be blank)
* an item is either a token or an expression
* a token is a string separable by whitespace
* an expression is a string containing arbitrary text separated by `(` and `)`
* occurences of `(` and `)` inside an expression must be escaped by `\`

#### Expression syntax

    (<type> <list of 0+ args separated by whitespace> <list of 0+ filters separated by whitespace>)
    (<upper case string>)
    (<list of args separated by '|'>)

where
* `(<upper case string>)` will be normalized to `(tag <upper case string>)`
* `(<list of args separated by '|'>)` will be normalized to `(oneof <list of tokens separated by whitespace>)`
* an arg is arbitrary text not containing whitespace with `+`, `(`, and `)` escaped with `\`
* a filter is `^\+[a-z0-9]+$` (note the `+` prefix)

#### Available expressions

TODO

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
