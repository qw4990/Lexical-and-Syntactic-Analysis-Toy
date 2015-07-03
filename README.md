词法和语法分析的练习程序

词法分析部分通过配置文件中的正则表达式， 去构建自动机， 依次来进行词法分析。

语法分析部分用LALR(1)对词法分析的结果进行分析。

下面是对配置文件的介绍， 见bfn.data
 # #号为注释
 # bfn配置文件
 # 整个配置文件分为两部分, 词法部分和语法部分
 #
 # 词法部分:
 #	格式一: % 匹配名 串1 串2 串3 ...
 #	比如 % if_words if else then
 #	那遇到if或者else或者then时, 就会被输出成如下
 #		if_words if
 #		if_words else
 #	更多见 lex.data
 #	
 #	格式二: $ 匹配名 正则串
 #	比如 $ number [0-9]+
 #	那遇到2312时, 就会输出如下
 #		number 2312
 #
 #
 # 语法部分:
 #	直接为bfn表达式, 详细例子见下面的C-文法
 #
 #
 #
 # 下面是一个C-的bfn范式 


 program -> stmt-sequence
 stmt-sequence -> stmt-sequence ; statement | statement
 statement -> if-stmt | repeat-stmt | assign-stmt | read-stmt | write-stmt
 if-stmt -> if exp then stmt-sequence end | if exp then stmt-sequence else stmt-sequence end
 repeat-stmt -> repeat stmt-sequence until exp
 assign-stmt -> identifier := exp
 read-stmt -> read identifier
 write-stmt -> write exp
 exp -> simple-exp comparison-op simple-exp | simple-exp
 comparison-op -> < | = | >
 simple-exp -> simple-exp addop term | term
 addop -> + | -
 term -> term mulop factor | factor
 mulop -> * | /
 factor -> ( exp ) | number | identifier 

 % reserve_words if then end else repeat until read write
 % operators := > < = + - * / ( ) ; { }
 $ number [-]?[0-9]+
 $ identifier [a-zA-Z_][a-zA-Z0-9]*


 #如果想要简单的支持分数, 就把下面两句话加上
 #$ fraction [0-9]+/[0-9]+
 #factor -> ( exp ) | number | identifier | fraction


接下来是C-的示例， 见code.data， 下面是一部分程序
l := 3/2;
read r;
repeat
	write l;
	l := l+1
until (l < r);


先对其进行词法分析， 分析后的结果存于lex.data中， 下面是一部分结果
1 identifier l
1 operators :=
1 number 3
1 operators /
1 number 2
1 operators ;
2 reserve_words read
2 identifier r
2 operators ;
3 reserve_words repeat


接着进行语法分析， 语法分析有多个步奏， 分别为
1.解析规则
2.生成自动机
3.产生状态表
4.语法分析


每个步奏都可以打印出相印结果， 比如下面是C-语法的一部分状态表
state :move	  until	 repeat	   read	 number	     if	identifier	   then	    end	   else	  write	      $	      -	      (
state0      	       	     s7	     s5	       	     s3	     s2	       	       	       	    s11	       	       	       
state1      	     r5	       	       	       	       	       	       	     r5	     r5	       	     r5	       	       
state2      	       	       	       	       	       	       	       	       	       	       	       	       	       
state3      	       	       	       	    s18	       	    s17	       	       	       	       	       	       	    s14
state4      	     r3	       	       	       	       	       	       	     r3	     r3	       	     r3	       	       
state5      	       	       	       	       	       	    s21	       	       	       	       	       	       	       
state6      	     r6	       	       	       	       	       	       	     r6	     r6	       	     r6	       	       
state7      	       	     s7	     s5	       	     s3	     s2	       	       	       	    s11	       	       	       
state8      	     r4	       	       	       	       	       	       	     r4	     r4	       	     r4	       	       
state9      	     r2	       	       	       	       	       	       	     r2	     r2	       	     r2	       	       
state10     	       	       	       	       	       	       	       	       	       	       	     ac	       	       
state11     	       	       	       	    s18	       	    s17	       	       	       	       	       	       	    s14



进行语法分析后， 如果成功， 则返回success；
否则返回错误的代码以及其行号。

PS：程序的错误处理策略是遇到错误， 反馈错误， 然后立即终结程序(exit)。
