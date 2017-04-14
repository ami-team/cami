/* Author : Jerome ODIER
 * Email : jerome.odier@lpsc.in2p3.fr
 */

/*-------------------------------------------------------------------------*/

#include <cstdlib>
#include <iostream>

#include "include/cami.h"

/*-------------------------------------------------------------------------*/

int main(int argc, char **argv)
{
	char *AMIUser = getenv("AMIUser");
	char *AMIPass = getenv("AMIPass");
	char *AMIHost = getenv("AMIHost");

	if(AMIUser == NULL
	   ||
	   AMIPass == NULL
	   ||
	   AMIHost == NULL
	 ) {
		std::cerr << "Environment variable 'AMIUser' and/or 'AMIPass' and/or 'AMIHost' not defined"
		          << std::endl
		;

		return 1;
	}

	try
	{
		AMIClient client(AMIHost, "/AMI/servlet/net.hep.atlas.Database.Bookkeeping.AMI.Servlet.FrontEnd", 443);

		std::map<std::string, std::string> arguments;

		arguments.insert(std::pair<std::string, std::string>(
			"amiLogin",
			"jodier"
		));

		arguments.insert(std::pair<std::string, std::string>(
			"AMIUser",
			AMIUser
		));

		arguments.insert(std::pair<std::string, std::string>(
			"AMIPass",
			AMIPass
		));

		std::string result = client.execute(
			"GetUserInfo",
			arguments
		);

		std::cout << result
		          << std::endl
		;
	}
	catch(std::exception const &e)
	{
		std::cerr << e.what()
		          << std::endl
		;
	}

	return 0;
}

/*-------------------------------------------------------------------------*/
