Phraser is a DSL for recognizing English phrases. It finds consecutive
lists of subsequences that are defined by lists of tokens with embedded
token-matching expressions. Expressions consist of a type, arguments,
and attribute filters.

Contents: \* `Demo <#demo>`__ \* `Expressions <#expressions>`__ \*
`All-at-once expressions <#all-at-once-expressions>`__ \* `Penn
part-of-speech tag <#penn-part-of-speech-tag>`__ — (tag *TAG*) or
(*TAG*) \* `Dynamic expressions <#dynamic-expressions>`__ \*
`Number <#number>`__ — (number *+type +polarity*) \* `Regular
expression <#regular-expression>`__ — (regex *regex*) \* `Precomputable
expressions <#precomputable-expressions>`__ \* `Custom token
group <#custom-token-group>`__ — (oneof *tokens...*) or
(*token1*\ \|\ *token2...*) \* `Personal
pronoun <#possessive-pronoun>`__ — (perspro *+case +gender +number
+person +personhood*) \* `Possessive determiner <#possessive-pronoun>`__
— (posdet *+gender +number +person +personhood*) \* `Possessive
pronoun <#possessive-pronoun>`__ — (pospro *+case +gender +number
+person +personhood*) \* `Possessive token <#possessive-token>`__ —
(pos) \* `Verb <#verb>`__ — (to *lemma +fieldtype +number +person*) \*
`Raw tokens <#raw-tokens>`__ \* `Configuration <#configuration>`__ \*
`Expression syntax <#expression-syntax>`__ \* `Phrase file
syntax <#phrase-file-syntax>`__ \* `Architecture <#architecture>`__ \*
`Preprocessing <#preprocessing>`__ \* `HTML entity
parsing <#html-entity-parsing>`__ \* `Destuttering <#destuttering>`__ \*
`Unicode to ASCII normalization <#unicode-to-ascii-normalization>`__ \*
`Sentence boundary detection <#sentence-boundary-detection>`__ \*
`Tokenization <#tokenization>`__ \* `Token
normalization <#token-normalization>`__ \* `Tagging <#tagging>`__ \*
`Contraction reversing <#contraction-reversing>`__ \* `Textspeak
normalization <#textspeak-normalization>`__

Demo
~~~~

This phrase file:

::

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

::

    The Borg will assimilate your ass.

Results in:

::

    TODO

Expressions
~~~~~~~~~~~

All expressions are checked for validity by the expression evaluator of
their type during initialization.

All-at-once expressions
^^^^^^^^^^^^^^^^^^^^^^^

All-at-once expressions require all the input tokens at once to make
their judgements about whether each of them is a match. Used for
filtering on Penn part-of-speech tags.

All-at-once expression evaluators contain an AnalyzeTokens() method
which generates some opaque metadata about each token, and an IsMatch()
method which makes a judgment about a token with metadata.

Penn part-of-speech tag
'''''''''''''''''''''''

-  ``(tag <uppercase Penn POS tag>)`` or ``(<uppercase Penn POS tag>)``

+-------------+--------------------------+
| Dimension   | Possible filter values   |
+=============+==========================+
| N/A         | N/A                      |
+-------------+--------------------------+

Dynamic expressions
^^^^^^^^^^^^^^^^^^^

Dynamic expressions are open-class. Each expression is evaluated against
each input token at call time.

Dynamic expression evaluators contain a MightMatch() method which may
rule out all expressions of its type.

Number
''''''

-  ``(number ...)``

+-------------+--------------------------+
| Dimension   | Possible filter values   |
+=============+==========================+
| class       | ``+float`` ``+int``      |
+-------------+--------------------------+
| polarity    | ``+neg`` ``+nonneg``     |
+-------------+--------------------------+

Regular expression
''''''''''''''''''

-  ``(regex <regex>)``

+-------------+--------------------------+
| Dimension   | Possible filter values   |
+=============+==========================+
| N/A         | N/A                      |
+-------------+--------------------------+

Precomputable expressions
^^^^^^^^^^^^^^^^^^^^^^^^^

Precomputable expressions are closed-class, so we enumerate every
possible match and put these matches (literal tokens) in a lookup table
during initialization.

Custom token group
''''''''''''''''''

-  ``(oneof <space-separated list of tokens>)``

+-------------+--------------------------+
| Dimension   | Possible filter values   |
+=============+==========================+
| N/A         | N/A                      |
+-------------+--------------------------+

Personal pronoun
''''''''''''''''

-  ``(perspro ...)``

+--------------+-----------------------------------+
| Dimension    | Possible filter values            |
+==============+===================================+
| case         | ``+obj`` ``+refl`` ``+subj``      |
+--------------+-----------------------------------+
| gender       | ``+female`` ``male`` ``neuter``   |
+--------------+-----------------------------------+
| number       | ``+plur`` ``+sing``               |
+--------------+-----------------------------------+
| person       | ``+1st`` ``+2nd`` ``+3rd``        |
+--------------+-----------------------------------+
| personhood   | ``person`` ``thing``              |
+--------------+-----------------------------------+

Possessive determiner
'''''''''''''''''''''

-  ``(posdet ...)``

+--------------+-----------------------------------+
| Dimension    | Possible filter values            |
+==============+===================================+
| gender       | ``+female`` ``male`` ``neuter``   |
+--------------+-----------------------------------+
| number       | ``+plur`` ``+sing``               |
+--------------+-----------------------------------+
| person       | ``+1st`` ``+2nd`` ``+3rd``        |
+--------------+-----------------------------------+
| personhood   | ``person`` ``thing``              |
+--------------+-----------------------------------+

Possessive pronoun
''''''''''''''''''

-  ``(pospro ...)``

+--------------+-----------------------------------+
| Dimension    | Possible filter values            |
+==============+===================================+
| case         | ``+obj`` ``+refl`` ``+subj``      |
+--------------+-----------------------------------+
| gender       | ``+female`` ``male`` ``neuter``   |
+--------------+-----------------------------------+
| number       | ``+plur`` ``+sing``               |
+--------------+-----------------------------------+
| person       | ``+1st`` ``+2nd`` ``+3rd``        |
+--------------+-----------------------------------+
| personhood   | ``person`` ``thing``              |
+--------------+-----------------------------------+

Possessive token
''''''''''''''''

-  ``(pos)``

+-------------+--------------------------+
| Dimension   | Possible filter values   |
+=============+==========================+
| N/A         | N/A                      |
+-------------+--------------------------+

Verb
''''

-  ``(to <verb lemma> ...)``

+--------------+--------------------------------------------------------------+
| Dimension    | Possible filter values                                       |
+==============+==============================================================+
| field type   | ``+lemma`` ``+past`` ``+pastpart`` ``+pres`` ``+prespart``   |
+--------------+--------------------------------------------------------------+
| number       | ``+plur`` ``+sing``                                          |
+--------------+--------------------------------------------------------------+
| person       | ``+1st`` ``+2nd`` ``+3rd``                                   |
+--------------+--------------------------------------------------------------+

Raw tokens
^^^^^^^^^^

Everything that is not an expression is a raw token which is matched
verbatim.

Configuration
~~~~~~~~~~~~~

Expression syntax
^^^^^^^^^^^^^^^^^

::

    (<type> <0+ whitespace-separated args> <0+ whitespace-separated filters>)

or

::

    (<upper case Penn POS tag>)

or

::

    (<2+ args separated by '|'>)

where \* ``(<upper case Penn POS tag>)`` will be normalized to
``(tag <upper case Penn POS tag>)`` \* ``(<2+ args separated by '|'>)``
will be normalized to ``(oneof <2+ args separated by '|'>)`` \* an arg
is arbitrary text not containing whitespace with ``+``, ``(``, and ``)``
escaped with ``\`` \* a filter is ``^\+[a-z0-9]+$`` (note the ``+``
prefix)

Phrase file syntax
^^^^^^^^^^^^^^^^^^

::

    <phrase name> = <1+ comma-separated subsequence names>
    <1+ newline-separated sequences>

a subsequence is

::

    <dash divider>
    <1+ newline-separated item lists>

where \* a phrase name is ``^[a-z ]+$`` \* a subsequence name is
``^[a-z ]+$`` \* subsequence names will be trimmed on both sides \* the
number of subsequence names must match the number of sequences \* a dash
divider is ``^\\-+$`` \* an item list is 0+ space-separated items (ie,
lines can be blank) \* an item is either a token or an expression \* a
token is a string separable by whitespace \* an expression is a string
containing arbitrary text separated by ``(`` and ``)`` \* occurences of
``(`` and ``)`` inside an expression must be escaped by ``\``

Architecture
~~~~~~~~~~~~

::

            Analyzer (cc/analysis/)
              |  \
              |   Frontend (cc/frontend/)
              |         \
              |          +--HTMLEntityParser (cc/frontend/html/)
              |          +--Destutterer (cc/frontend/destutter/)
              |          +--AsciiNormalizer (cc/frontend/ascii/)
              |          +--SentenceSplitter (cc/frontend/sbd/)
              |          +--Tokenizer (cc/frontend/tokenize/)
              |          +--Americanizer (cc/frontend/americanize/)
              |          +--Tagger (cc/frontend/tag/)
              |          +--Uncontractor (cc/frontend/contractions/)
              |          +--TextSpeakNormalizer (cc/frontend/textspeak/)
              /
            /
      PhraseDetector (cc/phrase_detection/)
         /    \
        /   EnglishExpressionEvaluator (cc/expression/)
       /                     \
    VectorMembership          +--PrecomputableEvaluators
    SequenceDetector          +--DynamicEvaluators
    (cc/sequence_detection/)
                              (cc/english/, cc/tagging/)

    SequenceDetector
    * EqualitySequenceDetector
    * VectorMembershipSequenceDetector

    ExpressionTypeEvaluator
    * PrecomputableEvaluator
    * DynamicEvaluator

Preprocessing
~~~~~~~~~~~~~

Raw text is transformed into tagged tokens for use by the phrase
detectors.

Conversions: ``HTML`` → ``Unicode`` → ``ASCII`` → ``list of tokens`` →
``list of (possible tokens, tag)``

We use code from
`LAPOS <www.logos.ic.i.u-tokyo.ac.jp/~tsuruoka/lapos/>`__ for
tokenization and especially tagging.

Some of the Unicode normalization and token normalization is designed to
behave like the Stanford parser.

1. HTML entity parsing
^^^^^^^^^^^^^^^^^^^^^^

Example: ``&copy; &#169; &#xA9;`` → ``© © ©``

2. Destuttering
^^^^^^^^^^^^^^^

Example: ``Whooooooooooooooa!!!!!!``\ → ``Whoooa!``

We drop overly repeated characters.

3. Unicode to ASCII normalization
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Essentially, we want to strip accents, map symbols to ASCII equivalents,
and use LaTeX quotes.

The following steps occur for all Unicode code points in any index below
in order to generate a static mapping:

1. Replace nonprintable ASCII with space (U+0020).
2. Normalize the various Unicode open/close quote styles to smart quotes
   (eg, ``«`` ``»`` to ``“`` ``”``)

-  `quotes.txt <https://github.com/knighton/phraser/blob/master/phraser/cc/tokenization/data_import/quotes.txt>`__

3. Normalize currency symbols to ``$`` and ``cents`` (to match WSJ
   training data)

-  `currencies.txt <https://github.com/knighton/phraser/blob/master/phraser/cc/tokenization/data_import/currencies.txt>`__

4. Convert smart quotes to spaced Penn Treebank tokens (eg, ``“`` ``”``
   to `````` ``''``)

-  `ptb\_smart\_quotes.txt <https://github.com/knighton/phraser/blob/master/phraser/cc/tokenization/data_import/ptb_smart_quotes.txt>`__

5. Decompose the Unicode code points according to NFKD

-  `nfc.txt <https://github.com/knighton/phraser/blob/master/phraser/cc/tokenization/data_import/nfc.txt>`__
   from ICU
-  `nfkc.txt <https://github.com/knighton/phraser/blob/master/phraser/cc/tokenization/data_import/nfkc.txt>`__
   from ICU

6. Replace non-ASCII Unicode code points with visually confusable code
   point sequences of type SA (same script, any case) that contain at
   least one ASCII code point

-  `confusables.txt <https://raw.githubusercontent.com/knighton/phraser/master/phraser/cc/tokenization/data_import/confusables.txt>`__
   from ICU

7.  Filter out non-ASCII characters.
8.  Join into a string.
9.  Condense spaces.
10. Drop parenthesized non-Latin characters that don't map to ASCII (eg,
    U+3208 ``㈈``).

4. Sentence boundary detection
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

We use a custom rule-based classifier written for web comments.

5. Tokenization
^^^^^^^^^^^^^^^

The result of the previous steps is then fed to the LAPOS tokenizer.

6. Token normalization
^^^^^^^^^^^^^^^^^^^^^^

We make some changes in order to match the tagger's training data.

1. Certain punctuation tokens are escaped (eg, ``(`` to ``-LRB-``)

-  `brackets.txt <https://github.com/knighton/phraser/blob/master/phraser/cc/tokenization/data_import/brackets.txt>`__

2. Commonwealth spellings are Americanized

-  `americanize.txt <https://github.com/knighton/phraser/blob/master/phraser/cc/tokenization/data_import/americanize.txt>`__

7. Tagging
^^^^^^^^^^

Respelled tokens are fed to the LAPOS tagger, which uses a model
pretrained on WSJ sections 2-21.

8. Contraction reversing
^^^^^^^^^^^^^^^^^^^^^^^^

We reverse contractions, using the part-of-speech tag to disambiguate
verb ``'s`` and possessive ``'s``. This results in multiple possible
words for some contractions (ie. 's = is/has, 'd = did/had/would/).

9. Textspeak normalization
^^^^^^^^^^^^^^^^^^^^^^^^^^

We list alternate forms of tokens ("ur" → "ur", "your", "you're").

--------------


TODO
~~~~
Important:

New features:

* "oneof" expressions.  Including the "|" syntax.

* Regex expressions.  Parsing those out of the phrase configs will be fun.
  Update expression syntax section of README.md when done.

* Multiple possible tags in tag expressions?

* Support "(TAG)" syntax.

* Implement (pos) expression type for ' and 's.

Correctness:

* List the possible Penn tags for checking tag expression arguments (ie, the tag
  the user wants to filter on) in TagEvaluator.

* Multi-token expressions.  Internally preprocess the precomputable expressions
  to generate normal single-token expressions.  Needed for some possessive
  personal edge cases to work.  Example:

      phrase config:   "I have (posdet +you) homework"

      internal config: "I have (posdet-1-of-1 +you) homework"
                       "I have (posdet-1-of-2 +you) (posdet-2-of-2 +you) homework"

      user input:      "I have yall's homework"

      tokenized:       "I have yall 's homework"

  I do not forsee having a use for non-precomputable multi-word expressions.

* Destuttering: Make it work on canonically equivalent code point sequences.
  Can't solve it by just NFC normalization, have to take combining diacritical
  marks into account.  Also do NFC normalization before calling it.

Requirements
~~~~~~~~~~~~

Phraser is a nonessential python extension around a C++ codebase that uses several C++11 features.  You'll need python and a recent C++ compiler.

We support clang++ (Ubuntu and Mac OS X) and g++ (Ubuntu only), going back to g++ 4.7 (released March 2012) and clang++ 3.4 (released January 2014).  Earlier clang versions could probably be supported by dropping some flags.  Further than that would require nontrivial code changes.

Currently, the build flags are much stricter when using clang.  It's geared toward development being done against clang, and deployment using g++ on an older system.

Release notes
~~~~~~~~~~~~~

Further planned
^^^^^^^

-  Destuttering handles bigrams ("hahahahaha" → "haha").
-  Destuttering handles symbols ("😋😋😋" → "😋").
-  Added basic textspeak normalization.

Planned
^^^^^^^

User visible:

* Phrase configs are now defined in YAML (before, a custom text format).
* Boolean operators on expressions are added (and, or, not, etc.).

Backend:

* Integrated a rule-based sentence boundary detector for web comments (before, assumed one sentence per input).
* English contractions are automatically replaced with their uncontracted equivalents.
* All-at-once expressions removed (use dynamic expressions instead).
* Tagging is now done automatically in the frontend.

0.1.9 (2015-05-31)
^^^^^^^^^^^^^^^^^^

- Fix release: Circle CI deployment.

0.1.8 (2015-05-30)
^^^^^^^^^^^^^^^^^^

- CircleCI deployment.

0.1.7 (2015-05-30)
^^^^^

- Testing CircleCI deployment.

0.1.6 (2015-05-28)
^^^^^^^^^^^^^^^^^^

- Fix release: fix build_ext for more recent Ubuntu releases.  Chooses compiler based on /etc/lsb-release.

0.1.5 (2015-05-27)
^^^^^^^^^^^^^^^^^^

- Fix release: setup.py defaults to building the python extension using g++-4.7 when not on Darwin in order for build_ext to work on an older system.  build_ext is now broken on python 2.7.8 due to flags setup.py automatically inserts.

0.1.4 (2015-05-27)
^^^^^^^^^^^^^^^^^^

- Add support for g++ 4.7 and 4.8 when on Linux (tested versions: 4.7.4-2ubuntu1, 4.8.3-12ubuntu3).

0.1.3 (2015-05-22)
^^^^^^^^^^^^^^^^^^

- Add support for g++ when on Linux (tested version: 4.9.1-16ubuntu6).

0.1.2 (2015-05-22)
^^^^^^^^^^^^^^^^^^

- Fix release: add graft command.

0.1.1 (2015-05-22)
^^^^^^^^^^^^^^^^^^

- Fix release: package the header files as well.

0.1.0 (2015-05-22)
^^^^^^^^^^^^^^^^^^

- Rewrite the python extension to return an object that contains the state, instead of calling init at the module level.
- Add valgrind invocations.

0.0.3 (2015-05-11)
^^^^^^^^^^^^^^^^^^

-  Fix release.

0.0.2 (2015-05-06)
^^^^^^^^^^^^^^^^^^

-  Phraser is now importable via pip as a python module.

0.0.1 (2015-05-06)
^^^^^^^^^^^^^^^^^^

-  Initial release. Written in C++11. Also builds a python extension.
   Compile with clang on Xubuntu or OS X. Tested versions:

Xubuntu:

::

      clang version 3.6.0 (trunk 223446)
      Target: x86_64-unknown-linux-gnu
      Thread model: posix

OS X:

::

      Apple LLVM version 6.0 (clang-600.0.57) (based on LLVM 3.5svn)
      Target: x86_64-apple-darwin13.4.0
      Thread model: posix
