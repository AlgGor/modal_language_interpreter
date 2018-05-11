#include <iostream>
#include <string>
#include <vector>
#include <cstdio>
#include <map>
#include <stack>  

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>

#include <fcntl.h>
using namespace std;

	/*


	
			Начало лексического анализатора




	*/

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

	Ident (string name_of_ident)
	{
		name= name_of_ident;
		declare = false;
		assign = false;
	}
};

vector<Ident> tabl_of_identifiers;					
vector<string> tabl_of_strings;

class Scanner
{
	/*vector<Ident> tabl_of_identifiers;					
	vector<string> tabl_of_strings;
	*/

	enum state { H, IDENTIFIER, NUMBER, LINE, COMMENT, COMPARE, DELIMITER, NOT_EQ};		
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
			throw 'f';			//  проблемы с файлом
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
					Ident current_ident(buf);
					tabl_of_identifiers.insert(tabl_of_identifiers.end(),current_ident);
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


	/*


	
			Начало синтаксического анализатора




	*/

 class Parser
{
	Lex 				curr_lex; // текущая лексема
	type_of_lex 		curr_type;
	int 				curr_value;
	Scanner 			scan;
	vector<type_of_lex> stack_of_lex;
	vector<int> 		stack_of_ident;

	void 	Program();	 	// 	процедуры РС-метода
	void	Descriptions_plus_Operators();

	void	Descriptions();
	void	Description();
	void 	Variable(type_of_lex);

	void 	Operators();
	void 	Operator();
	void 	Third_order_expression();
	void 	Second_order_expression();
	void 	First_order_expression();
	void 	Expression();
	/*void	D1();			// 	Descriptions_plus_Operators
	void    B();			//	Operators
	void	D();		 	//	Description
	void	S();			//  Operator
	void	E();			//	Third_order_expression
	void	E1();			//	Second_order_expression
	void	T();			//	First_order_expression
	void 	F();	 		//	Expression
	*/


	/*void 	//dec ( type_of_lex type);		// семантичиеские действия
	void	//check_id ();
	void	//check_op ();
	void	//check_not ();
	void	//eq_type ();
	void	////eq_bool ();
	void	//check_id_in_read ();
	void 	//dec ( type_of_lex type);
	*/
	
	void get_lexem () // получить очередную лексему
	{
		curr_lex = scan.get_lex();
		curr_type = curr_lex.get_type();
		curr_value = curr_lex.get_value();
	}

	public:
		//Poliz prog; // внутреннее представление программы
		Parser ( const char *program) : curr_lex(LEX_COMMENT), scan (program){}		/* ,prog (1000) */
		void 	analyze(); // анализатор с действиями
};



void Parser::analyze ()
{
	get_lexem ();
	Program();
	//prog.print();
	cout << endl << "Yes!!!" << endl;
}

void Parser::Program()
{
	if ( curr_type == LEX_PROGRAM )
		get_lexem ();
	else
		throw curr_lex;
	if ( curr_type == LEX_LEFT_CURLY_PAREN )
		get_lexem ();
	else
		throw curr_lex;

	Descriptions_plus_Operators();

	if ( curr_type == LEX_RIGHT_CURLY_PAREN )
		get_lexem ();
	else
		throw curr_lex;

	if ( curr_type != LEX_END_OF_PROGRAM )
		throw curr_lex;
}

void Parser::Descriptions_plus_Operators()
{		

		Descriptions();
		//Operators();		
}

void Parser::Descriptions(){

	if ( (curr_type==LEX_INT) || (curr_type==LEX_BOOLEAN) || (curr_type==LEX_STRING))
	{
		Description();
		while (curr_type==LEX_SEMICOLON){
			get_lexem();
			Description();
		}
	}
}

void Parser::Description ()
{
	type_of_lex current_type_of_variables;

	stack_of_ident.clear();
	if ( (curr_type==LEX_INT) || (curr_type==LEX_BOOLEAN) || (curr_type==LEX_STRING) )
	{
		current_type_of_variables= curr_type;
		get_lexem();
		if (curr_type==LEX_ID){
			Variable(current_type_of_variables);
			while ( curr_type == LEX_COMMA )
			{
				get_lexem();
				if (curr_type != LEX_ID)
					throw curr_lex;
				else {
					Variable(current_type_of_variables);
				}
			}
			//dec(current_type_of_variables);
		}else{
			throw curr_lex;
		}
	}
	
}

void Parser::Variable(type_of_lex current_type_of_variable)
{
	stack_of_ident.insert (stack_of_ident.begin(), curr_value );
	int value_of_variable=curr_value;

	get_lexem ();
	if (curr_type==LEX_ASSIGN)
	{
		get_lexem();
		if ((curr_type==LEX_LINE) && (current_type_of_variable==LEX_STRING))
		{ 
			cout<<"kek"<<endl;
			tabl_of_identifiers[value_of_variable].assign=true;
			tabl_of_identifiers[value_of_variable].value=curr_value;
			get_lexem();
		}
		else
		{
			if ((curr_type==LEX_NUMBER))
			{ 
				tabl_of_identifiers[value_of_variable].assign=true;
				tabl_of_identifiers[value_of_variable].value=curr_value;
				get_lexem();
			}
			else{
				throw curr_lex;
			}
		}
	}
}

void Parser::Operators()
{
	Operator();
	while ( curr_type == LEX_SEMICOLON )
	{
		get_lexem();
		Operator();
	}
}

void Parser::Operator ()
{
	//int pl0, pl1, pl2, pl3;
	if ( curr_type == LEX_IF )
	{
		get_lexem();
		Third_order_expression();
		//eq_bool();
		//pl2 = prog.get_free ();
		//prog.blank();
		//prog.put_lex (Lex(POLIZ_FGO));
		
		get_lexem();
		Operator();
		//pl3 = prog.get_free();
		//prog.blank();
		//prog.put_lex (Lex(POLIZ_GO));
		//prog.put_lex (Lex(POLIZ_LABEL, prog.get_free()),pl2);
		if (curr_type == LEX_ELSE)
		{
			get_lexem();
			Operator();
			//prog.put_lex(Lex(POLIZ_LABEL,prog.get_free()),pl3);
		}
		else
			throw curr_lex;
		
	} 
	else
		if ( curr_type == LEX_WHILE )
		{
			//pl0 = prog.get_free();
			get_lexem();
			Third_order_expression();
			//eq_bool();
			//pl1 = prog.get_free();
			//prog.blank();
			//prog.put_lex (Lex(POLIZ_FGO));
			if ( curr_type == LEX_DO )
			{
				get_lexem();
				Operator();
				//prog.put_lex (Lex (POLIZ_LABEL, pl0));
				//prog.put_lex (Lex ( POLIZ_GO));
				//prog.put_lex (Lex(POLIZ_LABEL, prog.get_free()),pl1);
			}
			else
				throw curr_lex;
		} 
		else
			if ( curr_type == LEX_READ )
			{
				get_lexem();
				if ( curr_type == LEX_LEFT_PAREN )
				{
					get_lexem();
					if ( curr_type == LEX_ID )
					{
						//check_id_in_read();
						//prog.put_lex (Lex ( POLIZ_ADDRESS, curr_value) );
						get_lexem();
					}
					else
						throw curr_lex;
					if ( curr_type == LEX_RIGHT_PAREN )
					{
						get_lexem();
						//prog.put_lex (Lex (LEX_READ));
					}
					else
						throw curr_lex;
				}
				else
					throw curr_lex;
			} //end read
			else
				if ( curr_type == LEX_WRITE )
				{
					get_lexem();
					if ( curr_type == LEX_LEFT_PAREN )
					{
						get_lexem();
						Third_order_expression();
						if ( curr_type == LEX_RIGHT_PAREN )
						{
							get_lexem();
							//prog.put_lex (Lex(LEX_WRITE));
						}
						else
							throw curr_lex;
					}
					else
						throw curr_lex;
				} //end write
				else
					if ( curr_type == LEX_ID )
					{
						//check_id ();
						//prog.put_lex (Lex(POLIZ_ADDRESS,curr_value));
						get_lexem();
						if ( curr_type == LEX_ASSIGN )
						{
							get_lexem();
							Third_order_expression(); //eq_type();
							//prog.put_lex (Lex (LEX_ASSIGN) );
						}
						else
							throw curr_lex;
					} //assign-end
					else 
						Operators();
}

void Parser::Third_order_expression ()
{
	Second_order_expression();
	if ( curr_type == LEX_EQ || curr_type == LEX_LESS || curr_type == LEX_GREATER || curr_type == LEX_LESS_EQ || curr_type == LEX_GREATER_EQ || curr_type == LEX_NOT_EQ )
	{
		stack_of_lex.insert (stack_of_lex.begin(),curr_type);
		get_lexem();
		Second_order_expression();
		//check_op();
	}
}

void Parser::Second_order_expression ()
{
	First_order_expression();
	while ( curr_type==LEX_PLUS || curr_type==LEX_MINUS || curr_type==LEX_OR )
	{
		stack_of_lex.insert (stack_of_lex.begin(),curr_type);
		get_lexem();
		First_order_expression();
		//check_op();
	}
}


void Parser:: First_order_expression ()
{
	Expression();
	while ( curr_type==LEX_MULTIP || curr_type==LEX_DIV || curr_type==LEX_AND )
	{
		stack_of_lex.insert (stack_of_lex.begin(),curr_type);
		get_lexem();
		Expression();
		//check_op();
	}
}

void Parser::Expression ()
{
	if ( curr_type == LEX_ID )
	{
		//check_id();
		//prog.put_lex (Lex (LEX_ID, curr_value));
		get_lexem();
	}
	else
		if ( curr_type == LEX_NUMBER )
		{
			stack_of_lex.insert (stack_of_lex.begin(), LEX_INT );
			//prog.put_lex ( curr_lex );
			get_lexem();
		}
		else
			if ( curr_type == LEX_TRUE )
			{
				stack_of_lex.insert (stack_of_lex.begin(), LEX_BOOLEAN );
				//prog.put_lex (Lex (LEX_TRUE, 1) );
				get_lexem();
			}
			else
				if ( curr_type == LEX_FALSE )
				{
					stack_of_lex.insert ( stack_of_lex.begin(),LEX_BOOLEAN );
					//prog.put_lex (Lex (LEX_FALSE, 0) );
					get_lexem();
				}
				else
					if ( curr_type == LEX_NOT )
					{
						get_lexem();
						Expression();
						//check_not();
					}
					else
						if ( curr_type == LEX_LEFT_PAREN )
						{
							get_lexem();
							Third_order_expression();
							if ( curr_type == LEX_RIGHT_PAREN)
								get_lexem();
							else
								throw curr_lex;
						}
						else
							throw curr_lex;
}






int main(){

	try{
		Parser    par("test");
		par.analyze();
	}
	catch (char exeption){
		if (exeption=='f'){
			cout<<"Bad file!"<<endl;
		}else{
			cout<<"Error!"<<endl;
		}
	}
	catch (Lex exeption){
		cout<<"Not so cool! "<< exeption <<endl;
	}
	
	
	

	return 0;
}