// LICENSE
//
//   This software is in the public domain. Where that dedication is not
//   recognized, you are granted a perpetual, irrevocable license to copy,
//   distribute, and modify this file as you see fit.

#ifndef FTS_FUZZY_MATCH_H
#define FTS_FUZZY_MATCH_H

#define ADJACENCY_BONUS  5// bonus for adjacent matches
#define SEPERATOR_BONUS  10// bonus if match occurs after a separator
#define CAMEL_BONUS 10// bonus if match is uppercase and prev is lower

#define LEADING_LETTER_PENALTY -3// penalty applied for every letter in str before the first match
#define MAX_LEADING_LETTERS 3// maximum penalty for leading letters
#define UNMATCHED_LETTER_PENALTY -1// penalty for every letter that doesn't matter
#define REDUNDANT_LETTER_PENALTY -3


#include <ctype.h> // for tolower
#include <stdbool.h>


// Returns true if each character in pattern is found sequentially within str
bool simple_fuzzy_match(char const * pattern, char const * str)
{
    while(*pattern != '\0' && *str != '\0') {
        if(tolower(*pattern) == tolower(*str))
            ++pattern;
        ++str;
    }

    return *pattern == '\0' ? true : false;
}

// Returns true if each character in pattern is found sequentially within str
// iff found then outScore is also set
int fuzzy_match(char const * pattern, char const * str)
{
    // Score consts

    // Loop variables
    int score = 0;
    char const * patternIter = pattern;
    char const * strIter = str;
    bool prevMatched = false;
    bool prevLower = false;
    bool prevSeparator = true; // true so if first letter match gets separator bonus

    // Loop over strings
    while(*patternIter != '\0' && *strIter != '\0') {
        const char patternLetter = *patternIter;
        const char strLetter = *strIter;

        if(tolower(patternLetter) == tolower(strLetter)) {
            // Apply penalty for each letter before the first pattern match
            // Note: std::max because penalties are negative values. So max is smallest penalty.
            if(patternIter == pattern) {
                int count = strIter - str;
                int penalty;
                if (count > MAX_LEADING_LETTERS) {
                    penalty = MAX_LEADING_LETTERS * LEADING_LETTER_PENALTY;
                } else {
                    penalty = count * LEADING_LETTER_PENALTY;
                }
                score += penalty;
            }

            // Apply bonus for consecutive bonuses
            if(prevMatched)
                score += ADJACENCY_BONUS;

            // Apply bonus for matches after a separator
            if(prevSeparator)
                score += SEPERATOR_BONUS;

            // Apply bonus across camel case boundaries
            if(prevLower && isupper(strLetter))
                score += CAMEL_BONUS;

            prevMatched = true;
            ++patternIter;
        } else {
            score += UNMATCHED_LETTER_PENALTY;
            prevMatched = false;
        }

        // Separators should be more easily defined
        prevLower = islower(strLetter);
        prevSeparator = strLetter == '_' || strLetter == ' ';

        ++strIter;
    }

    // Did not match full pattern
    if(*patternIter != '\0') {
        score -= REDUNDANT_LETTER_PENALTY;
    }

    return score;

}


#endif // FTS_FUZZY_MATCH_H
