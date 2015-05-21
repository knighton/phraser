import phraser
from memory_profiler import profile
from pkg_resources import resource_filename


@profile
def test_phraser():
    config = [resource_filename(__name__, "tests/threat_statement.txt")]
    runtime = phraser.Phraser(config)
    result1 = runtime.analyze(u'i will kill you.')
    assert result1.phrase_matches
    del result1
    result2 = runtime.analyze(u'blah blah some string')
    assert not result2.phrase_matches
    del result2
    del runtime
    del config


if __name__ == '__main__':
    test_phraser()
