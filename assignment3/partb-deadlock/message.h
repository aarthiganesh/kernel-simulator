struct message{
	char text[20]; // messages to send between processes
	char pin[4]; // pin for specified account number
	char accountnumber[6]; // account number of user
	char recipient[6]; // recipient for transfers
	float balance; // balance for account
	float withdrawal; // withdrawal request
};

