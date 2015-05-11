import phraser
import unittest
from pkg_resources import resource_filename


PHRASER_CONFIG = resource_filename(__name__, "./threat_statement.txt")

class TestPhraser(unittest.TestCase):
    def test_phraser_simple(self):
        result = phraser.init([PHRASER_CONFIG])
        self.assertEqual(True, result)
        text = u'blah blah some string'
        obj = phraser.analyze(text)
        self.assertEqual(text, obj.original_text)

    def test_phraser_threat1(self):
        result = phraser.init([PHRASER_CONFIG])
        self.assertEqual(True, result)
        text = u'i will kill you.'
        obj = phraser.analyze(text)
        self.assertNotEqual(obj.phrase_matches, [])
