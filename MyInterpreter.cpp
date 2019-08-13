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
	POLIZ_FALSE_GO		//46

	LEX_NOTED_IDENT		//47
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
		// впихнуть норм вывод через map
		 map<int,string> output=
		{
			{0,"LEX_PROGRAM"},	//0
			{1,"LEX_IF"},			//1
			{2,"LEX_ELSE"}, 	 	//2
			{3,"LEX_WHILE"},		//3
			{4,"LEX_FOR	"},		//4
			{5,"LEX_STEP"},		//5
			{6,"LEX_UNTIL"},		//6
			{7,"LEX_DO"}, 		//7
			{8,"LEX_GOTO"},		//8
			{9,"LEX_READ"},		//9
			{10,"LEX_WRITE"},		//10
			{11,"LEX_TRUE"},		//11
			{12,"LEX_FALSE"},		//12
			{13,"LEX_INT"},		//13
			{14,"LEX_STRING"},		//14
			{15,"LEX_BOOLEAN"},	//15

			{16,"LEX_ID"},			//16

			{17,"LEX_NOT"},		// not  	17
			{18,"LEX_MULTIP"},		// *		18
			{19,"LEX_DIV"},		// /		19
			{20,"LEX_MOD"},		// %		20
			{21,"LEX_PLUS"},		// +		21
			{22,"LEX_MINUS"},		// -		22
			{23,"LEX_LESS"}, 		// <		23
			{24,"LEX_GREATER"},		// >	24
			{25,"LEX_LESS_EQ"},		// <=	25
			{26,"LEX_GREATER_EQ"},		// >=	26
			{27,"LEX_EQ"},			// ==		27
			{28,"LEX_NOT_EQ"}, 		// !=	28
			{29,"LEX_AND"},		// and 		29
			{30,"LEX_OR"},			// or 		30
			{31,"LEX_ASSIGN"},		// = 		31

			{32,"LEX_LEFT_CURLY_PAREN"},	// { 		32
			{33,"LEX_RIGHT_CURLY_PAREN"},	// } 		33
			{34,"LEX_SEMICOLON"}, 			// ;		34
			{35,"LEX_COMMA"},				// "},		35
			{36,"LEX_COLON"}, 				// :		36
			{37,"LEX_LEFT_PAREN"},			// (		37
			{38,"LEX_RIGHT_PAREN"},		// ) 		38
			{39,"LEX_COMMENT"}, 			// /*  */	39

			{40,"LEX_NUMBER"},				//40
			{41,"LEX_LINE"},				//41
			{42,"LEX_END_OF_PROGRAM"},		//42		

			{43,"POLIZ_LABEL"},	//43
			{44,"POLIZ_ADDRESS"},	//44
			{45,"POLIZ_GO"},		//45
			{46,"POLIZ_FGO"},		//46
			{47,"LEX_NOTED_IDENT"}
		};
		
		s <<'('<<(output.find(l.t_lex))->second << ',' << l.value_lex << ");";
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
				{
					return Lex (it_map_identifier->second);
				}
				else
				{
					Ident current_ident(buf);
					for (unsigned int j=0; j<tabl_of_identifiers.size();j++ ){
						if (buf== (tabl_of_identifiers[j].name))
						{
							return Lex (LEX_ID, j);
						}
					}
					tabl_of_identifiers.push_back(current_ident);
					value=tabl_of_identifiers.size()-1;
					if (c!=':'){
						return Lex (LEX_ID, value);
					}else{

					}
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
	stack<type_of_lex>  stack_of_lex;
	type_of_lex 		current_type_of_variables;
	
	void 	Program();	 	// 	процедуры РС-метода
	void	Descriptions_labelus_Operators();

	void	Descriptions();
	void	Description();
	void 	Variable();

	void 	Operators();
	void 	Operator();

	void	Whole_expressionS();
	void 	Whole_expression();
	void	Sixth_priority_expression();
	void	Fifth_priority_expression ();
	void	Fourth_priority_expression ();
	void	Third_priority_expression();
	void 	Second_priority_expression();
	void 	First_priority_expression();
	void 	Expression();
	
	void 	Declare();		// семантичиеские действия
	void	Check_id_in_read();
	void	Check_ident ();
	void	Check_operators ();
	void	Check_not ();
	void	Eq_type ();
	void	Eq_bool ();
	void 	Eq_int();
	void 	Check_of_noted_ident();
	
	void get_lexem () // получить очередную лексему
	{
		
		curr_lex = scan.get_lex();
		while (curr_lex.get_type()==LEX_COMMENT){
			curr_lex = scan.get_lex();
		}
		curr_type = curr_lex.get_type();
		curr_value = curr_lex.get_value();
		
	}

	public:

		Parser ( const char *program) : curr_lex(LEX_COMMENT), scan (program){}		
		void 	analyze(); // анализатор с действиями
		void  	Print_poliz();
		vector<Lex> 		poliz;

		struct Noted_ident
		{
			Lex lex_ident;
			int adress;
			Noted_ident(){
				ident=Lex(LEX_ID);
				adress=0;
			}
		};


		vector<Noted_ident>			tabl_of_noted_operators;

};



void Parser::analyze ()
{
	get_lexem ();
	Program();
	//poliz.print();
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

	Descriptions_labelus_Operators();

	if ( curr_type == LEX_RIGHT_CURLY_PAREN )
		get_lexem ();
	else
		throw curr_lex;

	if ( curr_type != LEX_END_OF_PROGRAM )
		throw curr_lex;
}

void Parser::Descriptions_labelus_Operators()
{		

		Descriptions();
		Operators();		
}

void Parser::Descriptions()
{

	if ( (curr_type==LEX_INT) || (curr_type==LEX_BOOLEAN) || (curr_type==LEX_STRING))
	{
		Description();
		while ((curr_type==LEX_INT) || (curr_type==LEX_BOOLEAN) || (curr_type==LEX_STRING)){
			Description();
		}
	}
}

void Parser::Description ()
{

	if ( (curr_type==LEX_INT) || (curr_type==LEX_BOOLEAN) || (curr_type==LEX_STRING) )
	{
		current_type_of_variables= curr_type;
		get_lexem();
		if (curr_type==LEX_ID){
			Variable();
			while ( curr_type == LEX_COMMA )
			{
				get_lexem();
				if (curr_type != LEX_ID)
					throw curr_lex;
				else {
					Variable();
				}
			}
		}
		else
		{
			throw curr_lex;
		}
	}
	if (curr_type==LEX_SEMICOLON){
		get_lexem();
	}
	else
	{
		throw curr_lex;
	}

}

void Parser::Variable()
{
	if (curr_type==LEX_ID){
		Declare();
		poliz.push_back(Lex(POLIZ_ADDRESS,curr_value));
		get_lexem ();
		if (curr_type==LEX_ASSIGN)
		{
			get_lexem();
			if ((curr_type==LEX_LINE) && (current_type_of_variables==LEX_STRING))
			{ 
				poliz.push_back(Lex(current_type_of_variables,curr_value)); poliz.push_back(Lex(LEX_ASSIGN));
				get_lexem();
			}
			else
			{
				if ((current_type_of_variables==LEX_INT)&&(curr_type==LEX_NUMBER))
				{ 
					poliz.push_back(Lex(current_type_of_variables,curr_value)); poliz.push_back(Lex(LEX_ASSIGN));
					get_lexem();
				}
				else{
					if ((current_type_of_variables==LEX_BOOLEAN)&&( (curr_type==LEX_FALSE) || (curr_type==LEX_TRUE) ) )
					{
						if (curr_type==LEX_TRUE){ curr_value=1;}
						poliz.push_back(Lex(current_type_of_variables,curr_value)); poliz.push_back(Lex(LEX_ASSIGN));
						get_lexem();
					}else{
						throw curr_lex;
					}
				}
			}
		}else{
			poliz.pop_back();
		}
	}
	else
	{
		throw curr_lex;
	}
}

void Parser::Operators()
{
	if (curr_type!= LEX_RIGHT_CURLY_PAREN){
		Operator();
		while ( curr_type != LEX_RIGHT_CURLY_PAREN)
		{
			Operator();
		}
	}
	
}

void Parser::Operator ()
{
	int label1, label2, label3, label_for_goto;

	if ( curr_type == LEX_IF )
	{
		get_lexem();
		if (curr_type!=LEX_LEFT_PAREN)
			throw curr_lex;
		get_lexem();
		Whole_expression();
		Eq_bool();
		label1 = poliz.size ();
		poliz.push_back(Lex(POLIZ_LABEL));
		poliz.push_back(Lex(POLIZ_FALSE_GO));
		if (curr_type!=LEX_RIGHT_PAREN)
			throw curr_lex;
		get_lexem();
		Operator();		
		if (curr_type == LEX_ELSE)
		{
			label2=poliz.size();
			poliz.push_back(Lex(POLIZ_LABEL));
			poliz.push_back(Lex(POLIZ_GO));
			poliz[label1]=Lex(POLIZ_LABEL,poliz.size());	
			get_lexem();
			Operator();
			poliz[label2]=Lex(POLIZ_LABEL,poliz.size());
		}else{
			poliz[label1]=Lex(POLIZ_LABEL,poliz.size());
		}		
	} 
	else
		if ( curr_type == LEX_WHILE )
		{
			if (curr_type!=LEX_LEFT_PAREN)
				throw curr_lex;
			label1 = poliz.size();
			get_lexem();
			Whole_expression();
			Eq_bool();

			label2= poliz.size();
			poliz.push_back(Lex(POLIZ_LABEL));
			poliz.push_back(Lex(POLIZ_FALSE_GO));
			if (curr_type==LEX_RIGHT_PAREN)
			{
				get_lexem();
				Operator();
				poliz[label1]= Lex (POLIZ_LABEL);
				poliz.push_back(Lex ( POLIZ_GO));
				poliz[label2]=Lex(POLIZ_LABEL, poliz.size());
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
						Check_id_in_read();
						poliz.push_back(Lex ( POLIZ_ADDRESS, curr_value) );
						get_lexem();

					}
					else
						throw curr_lex;
					if ( curr_type == LEX_RIGHT_PAREN )
					{
						get_lexem();
						poliz.push_back (Lex (LEX_READ));
					}
					else
						throw curr_lex;
					if (curr_type!=LEX_SEMICOLON)
						throw curr_lex;
					get_lexem();
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
						Whole_expressionS();
						if ( curr_type == LEX_RIGHT_PAREN )
						{
							get_lexem();
							poliz.push_back (Lex(LEX_WRITE));
						}
						else
							throw curr_lex;
						if (curr_type!=LEX_SEMICOLON)
							throw curr_lex;
						get_lexem();
					}
					else
						throw curr_lex;
				} //end write
				else
					if (curr_type==LEX_FOR){
						get_lexem();
						
						if (curr_type==LEX_ID){
							if (tabl_of_identifiers[curr_value].type!=LEX_INT){
								throw "parameter is not integer";
							}
							int value_of_param=curr_value;
							poliz.push_back(Lex(POLIZ_ADDRESS,curr_value));
							get_lexem();
							if (curr_type!=LEX_ASSIGN){
								throw curr_lex;
							}
							get_lexem();
							Whole_expression();
							Eq_int();
							poliz.push_back(Lex(LEX_ASSIGN));
							label1=poliz.size();
							poliz.push_back(Lex(POLIZ_LABEL));
							poliz.push_back(Lex(POLIZ_GO));
							if (curr_type!=LEX_STEP){
								throw curr_lex;
							}
							label3=poliz.size();
							poliz.push_back(Lex(POLIZ_ADDRESS,value_of_param));
							poliz.push_back(Lex(LEX_ID,value_of_param));
							get_lexem();
							Whole_expression();
							Eq_int();
							poliz.push_back(Lex(LEX_PLUS));
							poliz.push_back(Lex(LEX_ASSIGN));
							if (curr_type!=LEX_UNTIL){
								throw curr_lex;
							}
							poliz.push_back(Lex(LEX_ID,value_of_param));
							get_lexem();
							Whole_expression();
							Eq_int();
							poliz.push_back(Lex(LEX_LESS_EQ));
							label2=poliz.size();
							poliz.push_back(Lex(POLIZ_LABEL));
							poliz.push_back(Lex(POLIZ_FALSE_GO));
							if (curr_type!=LEX_DO){
								throw curr_lex;
							}
							poliz[label2]=Lex(POLIZ_LABEL,poliz.size());
							get_lexem();
							Operator();
							poliz.push_back(Lex(POLIZ_LABEL,label3));
							poliz.push_back(Lex(POLIZ_GO));
							poliz[label1]=Lex(POLIZ_LABEL,poliz.size());
						}
						else
						{
							throw curr_lex;
						}
					}
					else
					{
						if (curr_type==LEX_GOTO){
							get_lexem();
							if (curr_type==LEX_ID){

								// сюда бы какой- нибудь чек
								string name= table_of_strings[curr_value];
								auto it=tabl_of_noted_operators.begin();
								while (it!=tabl_of_noted_operators.end()){
									if((((*it).lex_ident).curr_value)==curr_value){
										int lbl= (*it).adress;
										break;
									}
									it++;
								}

								poliz.push_back(Lex(POLIZ_LABEL,lbl));
								poliz.push_back(Lex(POLIZ_GO));
								get_lexem();
								if (curr_type!=LEX_SEMICOLON)
								{
									throw curr_lex;
								}
								get_lexem();
							}
							else
							{
								throw curr_lex;
							}

						}
						else
						{
							if (curr_type==LEX_LEFT_CURLY_PAREN)
								{
									get_lexem();
									Operator();
									while(curr_type!=LEX_RIGHT_CURLY_PAREN){
										Operator();
									}
									if (curr_type!=LEX_RIGHT_CURLY_PAREN)
										throw curr_lex;
									get_lexem();
								}
							else
							{									
								{
									Whole_expression();
									if (curr_type!=LEX_SEMICOLON){
										throw curr_lex;
									}
									get_lexem();
								}
							}
						}
					}
}


void Parser:: Whole_expressionS()
{
	Whole_expression();
	while (curr_type==LEX_COMMA){
		get_lexem();
		Whole_expression();
	}
}

void Parser:: Whole_expression ()
{
	Sixth_priority_expression();
	if ( curr_type == LEX_ASSIGN)
	{
		stack_of_lex.push (curr_type);
		get_lexem();
		Sixth_priority_expression();
		Check_operators();
	}else{
		if (curr_type==LEX_COLON)
		{
			
			Noted_ident curr_ident;
			curr_ident.lex_ident=poliz.top();
			poliz.pop();
			curr_ident.adress=poliz.size();
			poliz.push_back(Lex(LEX_NOTED_IDENT,tabl_of_noted_operators.size()));
			tabl_of_noted_operators.push_back(curr_ident);
			Operator();
		}
	}
}

void Parser:: Sixth_priority_expression ()
{
	Fifth_priority_expression();
	while ( curr_type == LEX_OR)
	{
		stack_of_lex.push(curr_type);
		get_lexem();
		Fifth_priority_expression();
		Check_operators();
	}
}

void Parser:: Fifth_priority_expression ()
{
	Fourth_priority_expression();
	while ( curr_type == LEX_AND)
	{
		stack_of_lex.push (curr_type);
		get_lexem();
		Fourth_priority_expression();
		Check_operators();
	}
}

void Parser:: Fourth_priority_expression ()
{
	Third_priority_expression();
	if ( (curr_type == LEX_EQ) || (curr_type == LEX_LESS) || (curr_type == LEX_GREATER) || (curr_type == LEX_LESS_EQ) || (curr_type == LEX_GREATER_EQ) || (curr_type == LEX_NOT_EQ) )
	{
		stack_of_lex.push (curr_type);
		get_lexem();
		Third_priority_expression();
		Check_operators();
	}
}

void Parser::Third_priority_expression()
{
	Second_priority_expression ();
	while ( (curr_type==LEX_PLUS) || (curr_type==LEX_MINUS))
	{
		stack_of_lex.push (curr_type);
		get_lexem();
		Second_priority_expression();
		Check_operators();
	}
}

void Parser::Second_priority_expression ()
{
	First_priority_expression();
	while ( (curr_type==LEX_MULTIP) || (curr_type==LEX_MOD) || (curr_type==LEX_DIV) )
	{
		stack_of_lex.push (curr_type);
		get_lexem();
		First_priority_expression();
		Check_operators();
	}
}


void Parser:: First_priority_expression ()
{
	Expression();
}

void Parser::Expression ()
{
	if ( curr_type == LEX_ID )
	{
		//Check_ident();
		poliz.push_back(Lex (LEX_ID, curr_value));
		get_lexem();
	}
	else
		if (curr_type == LEX_LINE)
		{
			stack_of_lex.push (LEX_STRING);
			poliz.push_back ( curr_lex );
			get_lexem();
		}
		else
		{
			if ( curr_type == LEX_NUMBER )
			{
				stack_of_lex.push ( LEX_INT );
				poliz.push_back ( curr_lex );
				get_lexem();
			}
			else
				if ( curr_type == LEX_TRUE )
				{
					stack_of_lex.push ( LEX_BOOLEAN );
					poliz.push_back (Lex (LEX_TRUE,1) );
					get_lexem();
				}
				else
					if ( curr_type == LEX_FALSE )
					{
						stack_of_lex.push ( LEX_BOOLEAN );
						poliz.push_back (Lex (LEX_FALSE, 0) );
						get_lexem();
					}
					else
						if ( curr_type == LEX_NOT )
						{
							get_lexem();
							Expression();
							Check_not();
						}
						else
							if ( curr_type == LEX_LEFT_PAREN )
							{
								get_lexem();
								Whole_expression();
								if ( curr_type == LEX_RIGHT_PAREN)
									get_lexem();
								else
									throw curr_lex;
							}
							else
								throw curr_lex;
		}
}



	/*


	
			Начало описания действий семантического анализатора




	*/



void Parser::Declare()
{
	if (tabl_of_identifiers[curr_value].declare== true) 
		throw "twice";
	tabl_of_identifiers[curr_value].declare=true;
	tabl_of_identifiers[curr_value].type=current_type_of_variables;
}

void Parser::Check_id_in_read()
{
	if ( !tabl_of_identifiers[curr_value].declare )
		throw "not declared";
	if (tabl_of_identifiers[curr_value].type==LEX_BOOLEAN)
		throw "boolean in read";
	
}

void Parser::Check_ident()
{
	if ( tabl_of_identifiers[curr_value].declare )
		stack_of_lex.push(tabl_of_identifiers[curr_value].type);
	else
		throw "not declared";
}

void Parser::Check_of_noted_ident()
{
	if ( !tabl_of_identifiers[curr_value].declare)
		throw "not declared noted ident";
	if ( tabl_of_identifiers[curr_value].type!=LEX_ID)
		throw "improper ident";
}


void Parser::Check_operators()
{
	type_of_lex t1, t2, op, t = LEX_INT, r = LEX_BOOLEAN;
	t2 = stack_of_lex.top(); stack_of_lex.pop();
	op = stack_of_lex.top(); stack_of_lex.pop();
	t1 = stack_of_lex.top(); stack_of_lex.pop();
	if (op==LEX_PLUS){
		if (t1==LEX_STRING){
			r=LEX_STRING;
		}else{
			r=LEX_INT;
		}
	}
	if( op==LEX_MINUS || op==LEX_MULTIP || op==LEX_DIV || op==LEX_MOD )
	{ 
		r = LEX_INT;
	}
	if ( op == LEX_OR || op == LEX_AND )
		t = LEX_BOOLEAN;
	if ( (t1 == t2) ){
		if ((t!=LEX_BOOLEAN)){
			if (t1!=LEX_BOOLEAN){
				stack_of_lex.push(r);
				poliz.push_back(Lex(op) );
			}else{
				throw "wrong types are in operation";
			}
		}else{
			if (t1==LEX_BOOLEAN){
				stack_of_lex.push(r);
				poliz.push_back(Lex(op) );
			}
			else
				throw "wrong types are in operation";
		}
	}
	else
		throw "wrong types are in operation";
}

void Parser::Check_not ()
{
	if (stack_of_lex.top() != LEX_BOOLEAN)
		throw "wrong type is in not";
	else
	{
		stack_of_lex.push (LEX_BOOLEAN);
		poliz.push_back(Lex(LEX_NOT));
	}
}

void Parser::Eq_type ()
{
	type_of_lex t1=stack_of_lex.top(); stack_of_lex.pop();
	type_of_lex t2=stack_of_lex.top(); stack_of_lex.pop();
	if ( t1!= t2 ) 
		throw "wrong types are in =";
}

void Parser:: Eq_bool ()
{
	if ( stack_of_lex.top() != LEX_BOOLEAN )
		throw "expression is not boolean";
}

void Parser::Eq_int()
{
	if ( stack_of_lex.top() != LEX_INT )
		throw "expression is not integer";
}


void Parser::Print_poliz()
{
	cout<<endl<<endl;
	for (auto it=(poliz.begin());it!=poliz.end();it++)	
		cout<<*it<<endl;
}


struct Operand 
{
	bool is_string = false, is_number = false;
	int number;
	string str;
};

class Executor 
{
	Lex current_element;

	public:
	Executor():current_element(LEX_LINE){}
	void execute (vector <Lex> poliz);
};

void Executor::execute(vector <Lex> poliz) 
{

	stack <Operand> stack_of_operands;
	unsigned int index = 0;
	while (index != poliz.size()) {
		current_element = poliz[index];
		switch (current_element.get_type()) {
			case LEX_BOOLEAN:
			case LEX_INT:
			case POLIZ_ADDRESS: 
			case POLIZ_LABEL: {
				Operand new_operand;
				new_operand.is_number = true;
				new_operand.number = current_element.get_value();
				stack_of_operands.push(new_operand);
				break;
			}
			case LEX_ID:
				if (tabl_of_identifiers[current_element.get_value()].assign){
					Operand new_operand;
					if (tabl_of_identifiers[current_element.get_value()].type == LEX_STRING) {
						new_operand.is_string = true;
						new_operand.str = tabl_of_strings[tabl_of_identifiers[current_element.get_value()].value];
					} else {
						new_operand.is_number = true;
						new_operand.number = tabl_of_identifiers[current_element.get_value()].value;
					}
					stack_of_operands.push(new_operand);
					break;
				} else {
					throw "POLIZ: indefinite identifier";
				}
			case LEX_STRING: {
				Operand new_operand;
				new_operand.is_string = true;
				new_operand.str = tabl_of_strings[current_element.get_value()];
				stack_of_operands.push(new_operand);
				break;
			}
			case POLIZ_GO:
				index = stack_of_operands.top().number - 1;
				stack_of_operands.pop();
				break;
			case POLIZ_FALSE_GO: {
				int new_index = stack_of_operands.top().number;
				stack_of_operands.pop();
				if (!stack_of_operands.top().number) 
					index = new_index - 1;
				stack_of_operands.pop();
				break;
			}
			case LEX_AND: {
				int operand2 = stack_of_operands.top().number; stack_of_operands.pop();
				int operand1 = stack_of_operands.top().number; stack_of_operands.pop();
				Operand new_operand;
				new_operand.is_number = true;
				new_operand.number = (operand1 && operand2);
				stack_of_operands.push(new_operand);
				break;
			}
			case LEX_OR: {
				int operand2 = stack_of_operands.top().number; stack_of_operands.pop();
				int operand1 = stack_of_operands.top().number; stack_of_operands.pop();
				Operand new_operand;
				new_operand.is_number = true;
				new_operand.number = (operand1 || operand2);
				stack_of_operands.push(new_operand);
				break;
			}
			case LEX_NOT: {
				int operand = stack_of_operands.top().number;
				stack_of_operands.pop();
				Operand new_operand;
				new_operand.is_number = true;
				new_operand.number = !operand;
				stack_of_operands.push(new_operand);
			}
			case LEX_WRITE:
				if (stack_of_operands.top().is_number)
					cout << stack_of_operands.top().number << endl;
				if (stack_of_operands.top().is_string)
					cout << stack_of_operands.top().str << endl;
				stack_of_operands.pop();
				break;
			case LEX_READ: {
				Ident& identifier = tabl_of_identifiers[stack_of_operands.top().number];
				if (identifier.type == LEX_INT) {
					cout << identifier.name <<": ";
					cin >> identifier.value;
					cout<<endl;
				} else if (identifier.type == LEX_STRING) {
					identifier.value = tabl_of_strings.size();
					cout << identifier.name << ":";
					string str;
					cin >> str;
					tabl_of_strings.push_back(str);
				} 
				identifier.assign = true;
				stack_of_operands.pop();
				break;
			}
			case LEX_ASSIGN: {
				Operand operand = stack_of_operands.top(); stack_of_operands.pop();
				Ident &identifier = tabl_of_identifiers[stack_of_operands.top().number]; stack_of_operands.pop();
				stack_of_operands.push(operand);
				if (operand.is_number) 
					identifier.value = operand.number;
				if (operand.is_string) {
					identifier.value = tabl_of_strings.size();
					tabl_of_strings.push_back(operand.str);
				}
				identifier.assign = true;
				break;
			}
			case LEX_PLUS: {
				Operand operand2 = stack_of_operands.top(); stack_of_operands.pop();
				Operand operand1 = stack_of_operands.top();	stack_of_operands.pop();
				Operand result;
				if (operand1.is_number) {
					result.is_number = true;
					result.number = operand1.number + operand2.number;
				}
				if (operand1.is_string) {
					result.is_string = true;
					result.str = operand1.str + operand2.str;
				}
				stack_of_operands.push(result);
				break;
			}
			case LEX_MINUS: {
				Operand operand2 = stack_of_operands.top();	stack_of_operands.pop();
				Operand operand1 = stack_of_operands.top();	stack_of_operands.pop();
				Operand result;
				result.is_number = true;
				result.number = operand1.number - operand2.number;
				stack_of_operands.push(result);
				break;
			}
			case LEX_MULTIP: {
				Operand operand2 = stack_of_operands.top(); stack_of_operands.pop();
				Operand operand1 = stack_of_operands.top();	stack_of_operands.pop();
				Operand result;
				result.is_number = true;
				result.number = operand1.number * operand2.number;
				stack_of_operands.push(result);
				break;
			}
			case LEX_DIV: {
				Operand operand2 = stack_of_operands.top(); stack_of_operands.pop();
				Operand operand1 = stack_of_operands.top();	stack_of_operands.pop();
				Operand result;
				result.is_number = true;
				result.number = operand1.number / operand2.number;
				stack_of_operands.push(result);
				break;
			}
			case LEX_MOD: {
				Operand operand2 = stack_of_operands.top();	stack_of_operands.pop();
				Operand operand1 = stack_of_operands.top();	stack_of_operands.pop();
				Operand result;
				result.is_number = true;
				result.number = operand1.number % operand2.number;
				stack_of_operands.push(result);
				break;
			}
			case LEX_LESS: {
				Operand operand2 = stack_of_operands.top();	stack_of_operands.pop();
				Operand operand1 = stack_of_operands.top();	stack_of_operands.pop();
				Operand result;
				result.is_number = true;
				if (operand1.is_number) 
					result.number = operand1.number < operand2.number;
				if (operand1.is_string)
					result.number = operand1.str < operand2.str;
				stack_of_operands.push(result);
				break;
			}
			case LEX_GREATER: {
				Operand operand2 = stack_of_operands.top(); stack_of_operands.pop();
				Operand operand1 = stack_of_operands.top(); stack_of_operands.pop();
				Operand result;
				result.is_number = true;
				if (operand1.is_number) 
					result.number = operand1.number > operand2.number;
				if (operand1.is_string)
					result.number = operand1.str > operand2.str;
				stack_of_operands.push(result);
				break;
			}
			case LEX_EQ: {
				Operand operand2 = stack_of_operands.top();	stack_of_operands.pop();
				Operand operand1 = stack_of_operands.top();	stack_of_operands.pop();
				Operand result;
				result.is_number = true;
				if (operand1.is_number) 
					result.number = operand1.number == operand2.number;
				if (operand1.is_string)
					result.number = operand1.str == operand2.str;
				stack_of_operands.push(result);
				break;
			}
			case LEX_NOT_EQ: {
				Operand operand2 = stack_of_operands.top();	stack_of_operands.pop();
				Operand operand1 = stack_of_operands.top();	stack_of_operands.pop();
				Operand result;
				result.is_number = true;
				if (operand1.is_number) 
					result.number = operand1.number != operand2.number;
				if (operand1.is_string)
					result.number = operand1.str != operand2.str;
				stack_of_operands.push(result);
				break;
			}
			case LEX_LESS_EQ: {
				Operand operand2 = stack_of_operands.top(); stack_of_operands.pop();
				Operand operand1 = stack_of_operands.top();	stack_of_operands.pop();
				Operand result;
				result.is_number = true;
				result.number = operand1.number <= operand2.number;
				stack_of_operands.push(result);
				break;
			}
			case LEX_GREATER_EQ: {
				Operand operand2 = stack_of_operands.top(); stack_of_operands.pop();
				Operand operand1 = stack_of_operands.top();	stack_of_operands.pop();
				Operand result;
				result.is_number = true;
				result.number = operand1.number >= operand2.number;
				stack_of_operands.push(result);
				break;
			}
			default: break;
		} //end switch
		++index;
	}; //end while
}

class Interpreter 
{
	Parser parser;
	Executor executor;

	public:
	Interpreter (char* program) : parser(program){};
	void Interpretation() {
		parser.analyze();
		executor.execute(parser.poliz);
	}
};




int main(int argc, char **argv )
{

	
	// первым аргументом нужно подать имя файла с тестом


	
	/*try{

		Scanner lexem(argv[1]);
		Lex x(LEX_LINE);
		do{
			x= lexem.get_lex();
			cout<<x<<endl;
		}while ((x.get_type())!=LEX_END_OF_PROGRAM);	
	}
	catch (char exeption){
		cout<<"Error!"<<endl;
	}
	*/
	
	/*

	try{
		Parser   par(argv[1]);
		par.analyze();
		par.Print_poliz();
	}
	catch (char exeption){
		if (exeption=='f'){
			cout<<"Bad file!"<<endl;
		}else{
			cout<<"Error!"<<endl;
		}
	}
	catch (Lex exeption){
		cout<<"Wrong lexem! "<< exeption <<endl;
	}
	catch (const char * exeption){
		cout<<exeption<<endl;
	}

	*/

	try {
		Interpreter interpreter(argv[1]);
		interpreter.Interpretation();
		cout<<"cool_program"<<endl
		return 0;
	}
	catch (type_of_lex type) {
		cout << "End of input" << endl;
		return 1;
	}
	catch (char c) {
		cout << "Error!" << c << endl;
		return 1;
	}
	catch (Lex lex) {
		if (lex.get_type() == LEX_ID) 
			cout<< "Wrong identifier " << tabl_of_identifiers[lex.get_value()].name << endl;
		else {
			cout << "Wrong lexeme: " << lex << endl;
		}
		return 1;
	}
	catch (const char* str) {
		cout << str << endl;
		return 1;
	}
	

	

	return 0;
}