import phraser
import unittest
from pkg_resources import resource_filename

PHRASER_CONFIG = resource_filename(__name__, "./threat_statement.txt")


class TestPhraser(unittest.TestCase):
    def test_phraser_simple(self):
        runtime = phraser.Phraser([PHRASER_CONFIG])
        self.assertTrue(bool(runtime))
        text = u'blah blah some string'
        obj = runtime.analyze(text)
        self.assertEqual(text, obj.original_text)

    def test_phraser_threat1(self):
        runtime = phraser.Phraser([PHRASER_CONFIG])
        self.assertTrue(bool(runtime))
        text = u'i will kill you.'
        obj = runtime.analyze(text)
        self.assertNotEqual(obj.phrase_matches, [])
