# exprevalc
A simple integer math expression evaluator written in C using a recursive descent parser implementation.
This project was created as teaching material for a friend so that I could showcase how a simple recursive descent parser can be implemented. The code could be of much higher quality if optimized for an specific usecase, so use at your discretion.

This implementation uses a basic tokenizer + parser combo to split the input string into a sequence of tokens and then parse through the tokens to calculate the final integer value of the expression. The expression evaluator respects the order of operations (PEMDAS, BODMAS, or however your generation calls it).

## Suppored features
- Simple decimal (base 10) integer literals
- Operators for addition and subtraction
- Operators for multiplication and division
- Parenthesis to create grouping expressions
- Integer arithmetic operations only

## Notes
### Integer only support
The code is just a showcase for teaching material, so I only implemented integer arithmetic, but it is trivial to modify the code to use, for example, double instead of int. Again, you are free to modify this code to fit your needs however you desire, this is just a showcase.

### Tokenization system implementation
For an usecase as simple as an arithmetic expression evaluator, I would probably have made a system where the current and previously parsed tokens were kept in memory, allowing the parser to be implemented in a way that it would have 0 heap allocations overhead.

I have decided against that for the sake of allowing the code to be reused with more complex user-defined token lists, which further helped showcase how this code could be derived into a parser for a more complex expression evaluation system or a full blown formal language.

Again, modifying the code to use said implementation is trivial, as the TokenList struct defines the entire logic of accessing and storing elements in memory, so it could very easily be modified to allow on-demand parsing to avoid heap allocations.

