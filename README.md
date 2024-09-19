# Dynamox C/C++ Developer Challenge

## Guilherme da Silva de Medeiros

This first commit is a MVP with all the main functional requirements along with some bonus requirements that were completed as well.
The whole code is commented and funcional.

## Running the project:



## IO patterns:

Here are some instructions in how to follow the main menu and send the inputs as this software understands:

### Matrix operation:
The matrix **must** be in as:

```a b c;d e f;g h i;``` 

For a matrix like:

```
|a b c|
|d e f|
|g h i|
```

Of course any size will work properly, this 3x3 matrix is just an example.

### Factorial operation:
As the factorial any integer number bellow 20 will work properly. This limit exists in this software because above 20 the result exceeds the capacity of the long long variable and it ends up giving an wrong answer.
Letters, strings, etc will be threated as a wrong input and the user will be asked to repeat the proccess.

## Attended requirements:

1 - Functional requirements
1. [ x ] Create an HMI where users can choose an operation and execute it.
1. [ x ] Users should be able to input single values and get the result.
1. [ x ] Users should be able to input array of values and get the result(If applicable).
1. [ x ] Add at least two operations
1. [ x ] Users must be able to select the operation type.
1. [ x ] One of the operations must be the calculation of the [Determinant](https://en.wikipedia.org/wiki/Determinant)

2 - Technical requirements
1. [ x ] Use C, C++ or both.
1. [ x ] The code must have a main.c file.
1. [ x ] Create libraries to organize the code.

3 - Bonus
1. [ ] Add possibility to create new operation type.
1. [ ] Add persistent operation log
1. [ x ] Enhance the determinant calculation functionality to allow users to select the dimensions of the matrix
1. [ x ] Detect an handle of errors on the operation input

## Next

I'm still working on the deploy and aftewards I will work on log operation, unity tests and possibly more operations