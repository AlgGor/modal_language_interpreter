#include <iostream>
#include <string>
#include <vector>
#include <cstdio>
#include <map> 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>

#include <fcntl.h>
using namespace std;

enum type_of_lex
{
	LEX_PROGRAM,	//0
	LEX_IF,			//1
	LEX_ELSE, 	 	//2
	LEX_WHILE,		//3
	LEX_FOR	,		//4
	LEX_STEP,		//5
	LEX_UNTIL,		//6
	LEX_DO, 		//7
	LEX_GOTO,		//8
	LEX_READ,		//9
	LEX_WRITE,		//10
	LEX_TRUE,		//11
	LEX_FALSE,		//12
	LEX_INT,		//13
	LEX_STRING,		//14
	LEX_BOOLEAN,	//15

	LEX_ID,			//16

	LEX_NOT,		// not  	17
	LEX_MULTIP,		// *		18
	LEX_DIV,		// /		19
	LEX_MOD,		// %		20
	LEX_PLUS,		// +		21
	LEX_MINUS,		// -		22
	LEX_LESS, 		// <		23
	LEX_GREATER,		// >	24
	LEX_LESS_EQ,		// <=	25
	LEX_GREATER_EQ,		// >=	26
	LEX_EQ,			// ==		27
	LEX_NOT_EQ, 		// !=	28
	LEX_AND,		// and 		29
	LEX_OR,			// or 		30
	LEX_ASSIGN,		// = 		31

	LEX_LEFT_CURLY_PAREN,	// { 		32
	LEX_RIGHT_CURLY_PAREN,	// } 		33
	LEX_SEMICOLON, 			// ;		34
	LEX_COMMA,				// ,		35
	LEX_COLON, 				// :		36
	LEX_LEFT_PAREN,			// (		37
	LEX_RIGHT_PAREN,		// ) 		38
	LEX_COMMENT, 			// /*  */	39

	LEX_NUMBER,				//40
	LEX_LINE,				//41
	LEX_END_OF_PROGRAM,		//42		

	POLIZ_LABEL,	//43
	POLIZ_ADDRESS,	//44
	POLIZ_GO,		//45
	POLIZ_FGO		//46
};

class Lex
{
	type_of_lex t_lex;
	int value_lex;

	public:
	
	Lex ( type_of_lex t, int v = 0)	{t_lex = t; value_lex = v;}

	type_of_lex get_type () { return t_lex; }

	int get_value () { return value_lex; }

	friend ostream& operator << ( ostream &s, Lex l )
	{
		s << '(' << l.t_lex << ',' << l.value_lex << ");";
		return s;
	}
};

struct Ident 											
{
	string name;
	bool declare;
	type_of_lex type;
	bool assign;
	int value;

	Ident ()
	{
		declare = false;
		assign = false;
	}
};


class Scanner
{
	vector<string> tabl_of_identifiers;					// объявить до main
	vector<string> tabl_of_strings;

	enum state { H, IDENTIFIER, NUMBER, LINE, COMMENT, COMPARE, DELIMITER, NOT_EQ};		// ALE заменилось на 
	map<string,type_of_lex> table_of_words=
	{
		{"program",LEX_PROGRAM},	
		{"if",LEX_IF},		
		{"else",LEX_ELSE}, 	
		{"while",LEX_WHILE},
		{"for",LEX_FOR},	
		{"step",LEX_STEP},	
		{"until",LEX_UNTIL},
		{"do",LEX_DO}, 		
		{"goto",LEX_GOTO},		
		{"read",LEX_READ},		
		{"write",LEX_WRITE},	
		{"true",LEX_TRUE},		
		{"false",LEX_FALSE},	
		{"int",LEX_INT},		
		{"string",LEX_STRING},	
		{"boolean",LEX_BOOLEAN},
		{"not",LEX_NOT},	
		{"and",LEX_AND},		
		{"or",LEX_OR}
	};
	map<string,type_of_lex> table_of_delimiters=
	{

		{"*",LEX_MULTIP},				// *
		{"/",LEX_DIV},					// /	
		{"%",LEX_MOD},					// %
		{"+",LEX_PLUS},					// +
		{"-",LEX_MINUS},				// -
		{"<",LEX_LESS}, 				// <
		{">",LEX_GREATER},				// >
		{"<=",LEX_LESS_EQ},				// <=
		{">=",LEX_GREATER_EQ},			// >=
		{"==",LEX_EQ},					// ==
		{"!=",LEX_NOT_EQ}, 				// !=
		{"=",LEX_ASSIGN},				// =

		{"{",LEX_LEFT_CURLY_PAREN},		// {
		{"}",LEX_RIGHT_CURLY_PAREN},	// }
		{";",LEX_SEMICOLON}, 			// ;
		{",",LEX_COMMA},				// ,
		{":",LEX_COLON}, 				// :
		{"(",LEX_LEFT_PAREN},			// (
		{")",LEX_RIGHT_PAREN},			// )
	};

	state CURRENT_STATE;
	bool check_end_of_program=false;
	int  fd;
	string buf;
	
	void get_char()
	{
		if (read(fd,&c,1)==0)
		{
			check_end_of_program=true;
		}
	}

	public:

	char c;

	Lex get_lex ();

	Scanner ( const char*  program )
	{
		if ( (fd = open ( program, O_RDONLY )) ==-1)
		{
			check_end_of_program=true;
		}else{
			buf.clear(); 
			get_char();
		}
		CURRENT_STATE = H;
	}
};


Lex Scanner::get_lex ()
{
	int value;
	
	CURRENT_STATE = H;
	
	do 					// проверка на конец ввода
	{	
		switch ( CURRENT_STATE )
		{
		case H:
			if ( check_end_of_program )
			{
				return Lex (LEX_END_OF_PROGRAM);
			}
			else if ( c ==' ' || c =='\n' || c=='\r' || c =='\t' )
			{
				get_char ();
			}
			else if ( isalpha(c) )
			{
				buf.clear ();	
				buf+=c;			
				get_char ();
				CURRENT_STATE =IDENTIFIER;
			}
			else if ( isdigit(c) )
			{
				value = c - '0';
				get_char ();
				CURRENT_STATE = NUMBER;
			}
			else if ( c== '/' )
			{
				get_char();
				CURRENT_STATE = COMMENT;
			}
			else if ( c== '=' || c== '<' || c== '>')
			{
				buf.clear ();
				buf+=c;	
				get_char ();
				CURRENT_STATE = COMPARE;
			}
			else if ( c == '!' )
			{
				buf.clear ();
				buf+=c;	
				get_char ();
				CURRENT_STATE =NOT_EQ ;
			}
			else if ( c== '"'){
				buf.clear ();
				buf+=c;	
				get_char ();
				CURRENT_STATE =LINE;
			}
			else
				CURRENT_STATE = DELIMITER;
			break;
		case IDENTIFIER:
			if (( isalpha(c) || isdigit(c) )&&(!check_end_of_program))
			{
				buf+=c;	
				get_char ();
			}
			else
			{
				auto it_map_identifier=table_of_words.find(buf);
				if ( (it_map_identifier)!=table_of_words.end() )
					return Lex (it_map_identifier->second);
				else
				{
					tabl_of_identifiers.insert(tabl_of_identifiers.end(),buf);
					value=tabl_of_identifiers.size()-1;
					return Lex (LEX_ID, value);
				}
			}
			break;
		case NUMBER:
			if (( isdigit(c) )&&(!check_end_of_program))
			{
				value= value* 10 + (c - '0');
				get_char();
			}
			else
				return Lex ( LEX_NUMBER, value);
			break;
		case COMMENT:
			if (c=='*')
			{
				get_char();
				while (true){
					if ((c!='*')&&(!check_end_of_program)){
						get_char();
					}else{
						if (!check_end_of_program){
							get_char();
							if (c=='/'){
								get_char();
								return Lex(LEX_COMMENT);
							}
						}else{
							throw '*';								// исключение из коммента
						}
					}
				}
			}
			else
			{
				return Lex(LEX_DIV);
			}
			break;
		case COMPARE:
			if (( c == '=' )&&(!check_end_of_program))
			{
				buf+=c;	
				get_char ();
				auto it_map_compare=table_of_delimiters.find(buf); 
				return Lex ( it_map_compare->second);
			}
			else
			{
				auto it_map_compare=table_of_delimiters.find(buf);
				return Lex (it_map_compare->second);
			}
			break;
		case NOT_EQ :
			if (( c == '=' )&&(!check_end_of_program))
			{
				buf+=c;	
				get_char ();
				return Lex ( LEX_NOT_EQ);
			}
			else
				throw '!';												// исключение из неравно
			break;
		case DELIMITER:
			{
				buf.clear ();
				buf+=c;	
				auto it_map_delimeter=table_of_delimiters.find(buf);
				if ( it_map_delimeter!=table_of_delimiters.end() )
				{
					get_char ();
					return Lex ( it_map_delimeter->second);
				}
				else
					throw c;
				break;
			}
		case LINE:
			if ((c!='"')&&(!check_end_of_program))
			{		
				buf+=c;	
				get_char ();
			}
			else
			{	
				if (!check_end_of_program){
					get_char();
					tabl_of_strings.insert(tabl_of_strings.end(),buf);
					value=tabl_of_strings.size()-1;
					return Lex ( LEX_LINE, value);
				}else{
					throw '"';
				}
			}
			break;
		}	// end switch
	}
	while ( true );
}

int main(){

	Scanner lexem("test");
	Lex x(LEX_LINE);

	try{
		do{
			x= lexem.get_lex();
			cout<<x<<endl;
		}while ((x.get_type())!=LEX_END_OF_PROGRAM);	
	}
	catch (char exeption){
		cout<<"Error!"<<endl;
	}
	

	return 0;
}