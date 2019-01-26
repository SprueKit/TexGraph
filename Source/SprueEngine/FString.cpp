#include "FString.h"

#include <sstream>
#include <iostream>
#include <iomanip>

using namespace SprueEngine;
using namespace std;

bool isPossessiveSRejecting(char character)
{
    return character == 's' || character == 'S' || character == 'z' || character == 'Z';
}

//Static Functions
void toUpper(basic_string<char>& s) {
   for (basic_string<char>::iterator p = s.begin(); p != s.end(); ++p)
      *p = toupper(*p); // toupper is for char
}

void toLower(basic_string<char>& s) {
   for (basic_string<char>::iterator p = s.begin(); p != s.end(); ++p)
      *p = tolower(*p);
}

void toTitleCase(basic_string<char>& s)
{
    bool hitSpace = true;
    for (basic_string<char>::iterator p = s.begin(); p != s.end(); ++p)
    {
        if (hitSpace && isalpha(*p))
        {
            *p = toupper(*p);
            hitSpace = false;
        }
        else if (isspace(*p))
            hitSpace = true;
    }
}

//FString constructors for 1 to 9 arguments
FString::FString(const char* str, const Variant& arg1)
{
    arguments_.push_back(arg1);
    evaluate(str);
    arguments_.clear();
}
FString::FString(const char* str, const Variant& arg1, const Variant& arg2)
{
    arguments_.push_back(arg1);
    arguments_.push_back(arg2);
    evaluate(str);
    arguments_.clear();
}
FString::FString(const char* str, const Variant& arg1, const Variant& arg2, const Variant& arg3)
{
    arguments_.push_back(arg1);
    arguments_.push_back(arg2);
    arguments_.push_back(arg3);
    evaluate(str);
    arguments_.clear();
}
FString::FString(const char* str, const Variant& arg1, const Variant& arg2, const Variant& arg3, const Variant& arg4)
{
    arguments_.push_back(arg1);
    arguments_.push_back(arg2);
    arguments_.push_back(arg3);
    arguments_.push_back(arg4);
    evaluate(str);
    arguments_.clear();
}
FString::FString(const char* str, const Variant& arg1, const Variant& arg2, const Variant& arg3, const Variant& arg4, const Variant& arg5)
{
    arguments_.push_back(arg1);
    arguments_.push_back(arg2);
    arguments_.push_back(arg3);
    arguments_.push_back(arg4);
    arguments_.push_back(arg5);
    evaluate(str);
    arguments_.clear();
}
FString::FString(const char* str, const Variant& arg1, const Variant& arg2, const Variant& arg3, const Variant& arg4, const Variant& arg5, const Variant& arg6)
{
    arguments_.push_back(arg1);
    arguments_.push_back(arg2);
    arguments_.push_back(arg3);
    arguments_.push_back(arg4);
    arguments_.push_back(arg5);
    arguments_.push_back(arg6);
    evaluate(str);
    arguments_.clear();
}
FString::FString(const char* str, const Variant& arg1, const Variant& arg2, const Variant& arg3, const Variant& arg4, const Variant& arg5, const Variant& arg6, const Variant& arg7)
{
    arguments_.push_back(arg1);
    arguments_.push_back(arg2);
    arguments_.push_back(arg3);
    arguments_.push_back(arg4);
    arguments_.push_back(arg5);
    arguments_.push_back(arg6);
    arguments_.push_back(arg7);
    evaluate(str);
    arguments_.clear();
}
FString::FString(const char* str, const Variant& arg1, const Variant& arg2, const Variant& arg3, const Variant& arg4, const Variant& arg5, const Variant& arg6, const Variant& arg7, const Variant& arg8)
{
    arguments_.push_back(arg1);
    arguments_.push_back(arg2);
    arguments_.push_back(arg3);
    arguments_.push_back(arg4);
    arguments_.push_back(arg5);
    arguments_.push_back(arg6);
    arguments_.push_back(arg7);
    arguments_.push_back(arg8);
    evaluate(str);
    arguments_.clear();
}
FString::FString(const char* str, const Variant& arg1, const Variant& arg2, const Variant& arg3, const Variant& arg4, const Variant& arg5, const Variant& arg6, const Variant& arg7, const Variant& arg8, const Variant& arg9)
{
    arguments_.push_back(arg1);
    arguments_.push_back(arg2);
    arguments_.push_back(arg3);
    arguments_.push_back(arg4);
    arguments_.push_back(arg5);
    arguments_.push_back(arg6);
    arguments_.push_back(arg7);
    arguments_.push_back(arg8);
    arguments_.push_back(arg9);
    evaluate(str);
    arguments_.clear();
}


//FString operations
//****************************************************************************
//
//  Function:       FString::evaluate(const string&)
//
//  Purpose:        Kicks off the process of generating the resulting string
//
//****************************************************************************
void FString::evaluate(const string& fmtString)
{
    resultString_.clear();
    resultString_.reserve(fmtString.length() * 3); //estimate
    for (string::const_iterator it = fmtString.begin(); it != fmtString.end(); ++it) {
        if (*it != '%') {
            resultString_.push_back(*it);
        } else {
            doArg(it, fmtString.end());
        }
    }
}

// Utility Enum
struct Comparison {
	enum compType {
		exist,
		notexist,
		less,
		lequal,
		gequal,
		equal,
		notequal,
		greater
	};
};


//****************************************************************************
//
//  Function:       FString::doCondition(string::const_iterator&, string::const_iterator)
//
//  Purpose:        Handles conditional expressions
//
//****************************************************************************
void FString::doCondition(string::const_iterator& it, string::const_iterator end)
{
	Comparison::compType comparison = Comparison::exist;
	bool value = false;
	int opA = 0, opB = 0;
    ++it;
	if (*it == '!') {
		++it;
		comparison = Comparison::notexist;
		if (numCheck(*it))
			opA = numCheck(*it);
	} else {
		if (numCheck(*it))
			opA = numCheck(*it);
		++it;
		if (*it == '>') {
			++it;
			if (*it == '=') {
				comparison = Comparison::gequal;
				++it;
				opB = numCheck(*it);
			} else {
				comparison = Comparison::greater;
				opB = numCheck(*it);
			}
		} else if (*it == '<') {
			++it;
			if (*it == '=') {
				comparison = Comparison::lequal;
				++it;
				opB = numCheck(*it);
			} else {
				comparison = Comparison::less;
				opB = numCheck(*it);
			}
		} else if (*it == '=') {
			++it;
			if (*it == '=') //don't care
				++it;
			comparison = Comparison::equal;
			opB = numCheck(*it);
		} else if (*it == '!') {
			++it;
			if (*it == '=') //don't care
				++it;
			comparison = Comparison::notequal;
			opB = numCheck(*it);
		}
	}

	bool goAhead = false;
	if (comparison == Comparison::exist || comparison == Comparison::notexist)
		goAhead = rangeCheck(opA);
	else
		goAhead = rangeCheck(opA) && rangeCheck(opB);

	if (goAhead) {
		if (comparison == Comparison::exist) {
			if (!arguments_[opA-1].ConvertToString().empty())
				value = true;
			else
				value = false;
		} else if (comparison == Comparison::notexist) {
			if (arguments_[opA-1].ConvertToString().empty())
				value = false;
			else
				value = true;
		} else if (comparison == Comparison::less) {
			if (arguments_[opA-1] < arguments_[opB-1])
				value = true;
			else
				value = false;
		} else if (comparison == Comparison::lequal) {
			if (arguments_[opA-1] <= arguments_[opB-1])
				value = true;
			else
				value = false;
		} else if (comparison == Comparison::equal) {
			if (arguments_[opA-1] == arguments_[opB-1])
				value = true;
			else
				value = false;
		} else if (comparison == Comparison::gequal) {
			if (arguments_[opA-1] >= arguments_[opB-1])
				value = true;
			else
				value = false;
		} else if (comparison == Comparison::greater) {
			if (arguments_[opA-1] > arguments_[opB-1])
				value = true;
			else
				value = false;
		} else if (comparison == Comparison::notequal) {
			if (arguments_[opA-1] != arguments_[opB-1])
				value = false;
			else
				value = true;
		}

		++it;
		if (value) {
			if (*it == '?') {
				++it;
				for ( ; *it != ':' && it != end; ++it) {
					if (*it == '%')
						doArg(it, end);
					else
						resultString_.push_back(*it);
				}
				++it;
                for (; *it != ';' && it != end; ++it); // skips
			}
		} else {
			if (*it == '?') {
				++it;
				for ( ; *it != ':' && it != end; ++it); //skip
				++it;
                for (; *it != ';' && it != end; ++it) {
					if (*it == '%')
						doArg(it, end);
					else
						resultString_.push_back(*it);
				}
			}
		}
	}
}


//****************************************************************************
//
//  Function:       FString::rangeCheck(int idx)
//
//  Purpose:        Determines if the argument # is within the bounds of the actual parameters
//
//  Return:         True if valid
//
//****************************************************************************
bool FString::rangeCheck(int idx)
{
    return (idx > 0 && idx <= (int)arguments_.size()) ? true : false;
}


//****************************************************************************
//
//  Function:       FString::doArg(string::const_iterator&, string::const_iterator)
//
//  Purpose:        Checks a character and determines what to do with it
//
//****************************************************************************
void FString::doArg(string::const_iterator& it, string::const_iterator end)
{
    Justify justify = None;
    int justifyChars = 0;
    if (*it == '%') {
        ++it;
        if (numCheck(*it)) {
            writeVar(numCheck(*it), false, false, justify, justifyChars);
        } else if (*it == ':') {
            resultString_.push_back(*it);
        } else if (*it == 'x' || *it == 'X') {
            doHex(it, end);
        } else if (*it == '?') {
            doCondition(it, end);
        } else if (*it == 'a') { //lowercase
            ++it;
            if (numCheck(*it)) {
                writeVar(numCheck(*it), true, false, justify, justifyChars);
            }
        } else if (*it == 'A') { //uppercase
            ++it;
            if (numCheck(*it)) {
                writeVar(numCheck(*it), true, true, justify, justifyChars);
            }
        }
        else if (*it == 't') { // title case
            ++it;
            if (numCheck(*it))
            {
                std::string append = getVarText(numCheck(*it), false, false, None, 0);
                toTitleCase(append);
                resultString_ += append;
            }
        }
        else if (*it == 'o')
        {
            ++it;
            std::string text = getVarText(numCheck(*it), false, false, None, 0);
            if (text.length() > 0 && isPossessiveSRejecting(text[text.length() - 1]))
                text += "'";
            else
                text += "'s";
            resultString_ += text;
        }
        else if (*it == '%') // Escaped %
            resultString_ += '%';
        else if (*it == ';') // Escaped ternary closure ;
            resultString_ += ';';
        else if (*it == 'l' || *it == 'r') { // left|right justifty
            justify = *it == 'l' ? Left : Right;
            ++it;
            while (*it != '-')
            {
                justifyChars *= 10;
                justifyChars += *it - '0';
                it += 1;
            }
            ++it;
            if (numCheck(*it))
                resultString_ += getVarText(numCheck(*it), false, false, justify, justifyChars);
        }
    } else {
        //error
		resultString_ += "!Argument Error!\n";
    }
}


//****************************************************************************
//
//  Function:       FString::writeVar(int)
//
//  Purpose:        Writes an argument into the resultant string
//
//****************************************************************************
void FString::writeVar(int id, bool caseOverride, bool upper, FString::Justify justify, int chars)
{
    std::string val = getVarText(id, caseOverride, upper, justify, chars);
    resultString_ = resultString_ + val.c_str();
}

std::string FString::getVarText(int id, bool caseOverride, bool upper, FString::Justify justify, int characters)
{
    if (rangeCheck(id)) {
        string str = arguments_[id - 1].ConvertToString();
        if (caseOverride) {
            if (upper)
                toUpper(str);
            else
                toLower(str);
        }

        if (justify != None)
        {
            std::stringstream ss;
            if (justify == Left)
                ss << std::left << std::setw(characters) << str;
            else
                ss << std::setw(characters) << str;
            return ss.str();
        }
        return str;
    }
    if (justify == None)
        return string();
    else
        return string(characters, ' ');
}


//****************************************************************************
//
//  Function:       FString::doHex(string::const_iterator&, string::const_iterator)
//
//  Purpose:        Outputs a parameter in hexadecimal format
//
//****************************************************************************
void FString::doHex(string::const_iterator& it, string::const_iterator end)
{
    bool caps = true;
    if (*it == 'x')
        caps = false;
    ++it;
    if (numCheck(*it) && rangeCheck(numCheck(*it))) {
        int arg = numCheck(*it) - 1;
        int value = arguments_[arg].getAsIntValue();
        
		stringstream ss;
		ss.unsetf(std::ios::dec | std::ios::oct | std::ios::showbase);
		ss.setf(std::ios::hex);
		if (caps)
			ss.setf( std::ios::uppercase );
		ss << value;
		string tStr = ss.str();
		resultString_ += tStr;
    }
}


//****************************************************************************
//
//  Function:   FString::numCheck(const char)
//
//  Purpose:    Utility function that checks if a character is valid argument #
//
//  Return:     1-9 if Success
//
//****************************************************************************
int FString::numCheck(const char a)
{
    if (atoi(&a) > 0 && atoi(&a) <= 9)
		return atoi(&a);
    return 0;
}