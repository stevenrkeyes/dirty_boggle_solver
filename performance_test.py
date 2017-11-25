import random

import subprocess

random.seed(42)

# English letters by frequency; source: Cornell University Math Explorer's Project
# https://www.math.cornell.edu/~mec/2003-2004/cryptography/subs/frequencies.html
letter_weights = {'E': 12.02,
                  'T': 9.1,
                  'A': 8.12,
                  'O': 7.68,
                  'I': 7.31,
                  'N': 6.95,
                  'S': 6.28,
                  'R': 6.02,
                  'H': 5.92,
                  'D': 4.32,
                  'L': 3.98,
                  'U': 2.88,
                  'C': 2.71,
                  'M': 2.61,
                  'F': 2.3,
                  'Y': 2.11,
                  'W': 2.09,
                  'G': 2.03,
                  'P': 1.82,
                  'B': 1.49,
                  'V': 1.11,
                  'K': 0.69,
                  'X': 0.17,
                  'Q': 0.11,
                  'J': 0.1,
                  'Z': 0.07,
                  }

# https://stackoverflow.com/questions/3679694/a-weighted-version-of-random-choice
def weighted_choice(choices):
    # Each choice is represented by a segment of the range (0, total);
    total = sum(choices.values())
    # randomly select a value r in that range
    r = random.uniform(0, total)
    # Find which choice's segment r has landed in
    upto = 0
    for c in choices.keys():
    	w = choices[c]
        if upto + w >= r:
            return c
        upto += w
    assert False, "Shouldn't get here"

def test_solve_time():
	# Generate a boggle board with letters roughly the same distribution as in English
	boggle_letters = "\n".join([weighted_choice(letter_weights) for i in range(16)])

	p = subprocess.Popen(['./solver'], stdout = subprocess.PIPE, stdin = subprocess.PIPE, stderr = subprocess.PIPE)
	stdout_data = p.communicate(input = boggle_letters)[0]

	start = stdout_data.find("Executed in ") + len("Executed in ")
	end = stdout_data.find(" milliseconds")

	#print stdout_data[stdout_data.find("Words found:"):-1]
	return int(stdout_data[start:end])

num_trials = 100

total = 0
for i in range(num_trials):
	total += test_solve_time()

average_solve_time = total * 1.0 / num_trials

print "Average solve time for " + str(num_trials) + " trials: " + str(average_solve_time) + " milliseconds"
