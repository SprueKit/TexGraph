#pragma once
#include <string>
#include <vector>

/********************************************************************************
    FString Usage:
        % is the escape character
            %[1-9] uses an argument in the current location
            %% places a %
            %; places a ; when inside of a ternary
        %[x|X][1-9] uses an argument in hexadecimal format
            'x' indicating lowercase and 'X' indicating uppercase for 10-15
        %[a|A][1-9] uses with its character case overriden to upper or lower
            'a' inidicating lowercase and 'A' indicating uppercase
        %[t][1-9] indicates proper case (first letter of each word capitalized)
        %[l][Ct]-[1-9] left justify for a width of Ct characters
        %[r][Ct]-[1-9] right justify for a width of Ct characters
        %[o][1-9] indicates possessive "'s" or "'" is to be appended
                e.g. FString("That's %o1 cat!", "Jon")
                        results: "That's Jon's cat!"
                but: FString("%o1 %?!2:not:: here!", "Phillis", false)
                        results: "Phillis' here!"
        %[s][PARAMS][1-9] indicates a smart insertion
            [PARAMS]
                p - plural
				o - possessive
                a - article, assumes a sequence of argument indices
                    %sa23 will use Arg 2 for the article quantity, arg three is the source

        Conditionals:
            Basic structure:
                %?[CONDITIONS]?[case true]:[case false];
            %? indicates a condition
            %?[1-9] is true if 'non-zero' condition
            %?![1-9] is true if 'zero' condition
            %?[1-9][comparison][1-9] compares two arguments
                    >, >=, ==, <, <= are the supported comparisons

                NOTE: to use a semi-colon : in a condition use the "%:" escape sequence,
				
		Lingual:
			%s[1-9] indicates singular if smart string
			%p[1-9] indicates plural if smart string
			%?m[1-9] asks for the gender (masc = true, fem = false, masc is default)

********************************************************************************/

#include <SprueEngine/Variant.h>

class FString
{
private:
    enum Justify {
        None,
        Left,
        Right
    };

    void evaluate(const std::string& fmtString);
    bool rangeCheck(int);
    void doArg(std::string::const_iterator& it, std::string::const_iterator end);
    void doHex(std::string::const_iterator& it, std::string::const_iterator end);
    void doCondition(std::string::const_iterator& it, std::string::const_iterator end);
    int numCheck(const char);
    void writeVar(int, bool, bool, Justify justification, int characters);
    std::string getVarText(int, bool, bool, Justify justification, int characters);

    std::vector< SprueEngine::Variant > arguments_;
    std::string resultString_;
public:
    FString(const char*, const SprueEngine::Variant&);
    FString(const char*, const SprueEngine::Variant&, const SprueEngine::Variant&);
    FString(const char*, const SprueEngine::Variant&, const SprueEngine::Variant&, const SprueEngine::Variant&);
    FString(const char*, const SprueEngine::Variant&, const SprueEngine::Variant&, const SprueEngine::Variant&, const SprueEngine::Variant&);
    FString(const char*, const SprueEngine::Variant&, const SprueEngine::Variant&, const SprueEngine::Variant&, const SprueEngine::Variant&, const SprueEngine::Variant&);
    FString(const char*, const SprueEngine::Variant&, const SprueEngine::Variant&, const SprueEngine::Variant&, const SprueEngine::Variant&, const SprueEngine::Variant&, const SprueEngine::Variant&);
    FString(const char*, const SprueEngine::Variant&, const SprueEngine::Variant&, const SprueEngine::Variant&, const SprueEngine::Variant&, const SprueEngine::Variant&, const SprueEngine::Variant&, const SprueEngine::Variant&);
    FString(const char*, const SprueEngine::Variant&, const SprueEngine::Variant&, const SprueEngine::Variant&, const SprueEngine::Variant&, const SprueEngine::Variant&, const SprueEngine::Variant&, const SprueEngine::Variant&, const SprueEngine::Variant&);
    FString(const char*, const SprueEngine::Variant&, const SprueEngine::Variant&, const SprueEngine::Variant&, const SprueEngine::Variant&, const SprueEngine::Variant&, const SprueEngine::Variant&, const SprueEngine::Variant&, const SprueEngine::Variant&, const SprueEngine::Variant&);

    //operator const std::string&() const {return resultString_;}
    //operator std::string&() {return resultString_;}
    operator const std::string() const {return resultString_;}
    //operator std::string() {return resultString_;}

    const std::string& str() const {return resultString_;}
    const char* c_str() const {return resultString_.c_str();}
    size_t length() const {return resultString_.length();}
};