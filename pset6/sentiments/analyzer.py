import nltk

class Analyzer():
    """Implements sentiment analysis."""

    def __init__(self, positives, negatives):
        """Initialize Analyzer."""
        self.tokenizer = nltk.tokenize.TweetTokenizer()
        self.positives = self._read_file(positives)
        self.negatives = self._read_file(negatives)

    def _read_file(self, filename):
        with open(filename, "r") as f:
            content = f.readlines()
            return [x.strip("\n") for x in content if not x[0] == ";" and not x == ""]
    
    def analyze(self, text):
        """Analyze text for sentiment, returning its score."""
        result = 0
        for word in self.tokenizer.tokenize(text):
            if word.lower() in self.positives:
                result += 1
            elif word.lower() in self.negatives:
                result -= 1
        return result
