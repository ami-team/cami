<a href="http://lpsc.in2p3.fr/" target="_blank">
	<img src="http://www.cern.ch/ami/images/logo_lpsc.gif" alt="LPSC" height="62" />
</a>
&nbsp;&nbsp;&nbsp;&nbsp;
<a href="http://www.in2p3.fr/" target="_blank">
	<img src="http://www.cern.ch/ami/images/logo_in2p3.gif" alt="IN2P3" height="72" />
</a>
&nbsp;&nbsp;&nbsp;&nbsp;
<a href="http://www.univ-grenoble-alpes.fr/" target="_blank">
	<img src="http://www.cern.ch/ami/images/logo_uga.png" alt="UGA" height="72" />
</a>
&nbsp;&nbsp;&nbsp;&nbsp;
<a href="http://home.cern/" target="_blank">
	<img src="http://www.cern.ch/ami/images/logo_cern.png" alt="CERN" height="72" />
</a>
&nbsp;&nbsp;&nbsp;&nbsp;
<a href="http://atlas.cern/" target="_blank">
	<img src="http://www.cern.ch/ami/images/logo_atlas.png" alt="ATLAS" height="87" />
</a>

CAMI
====

CAMI is an AMI C++ client. It was originally developed for the A Toroidal LHC ApparatuS (ATLAS) experiment, one of the two general-purpose detectors at the Large Hadron Collider (LHC).

Installing CAMI
===============

 * Requierments:

Make sure that [GCC](https://gcc.gnu.org/) (or [Clang](https://clang.llvm.org/)) and [OpenSSL](https://www.openssl.org/) are installed:
```bash
g++ --version
openssl version
```

 * Compiling, testing and installing:
```bash
make
make test
make install
```

Installed files: `/usr/local/include/cami.h` and `/usr/local/lib/libcami.a`.

Example
=======

```c++
try
{
	AMIClient client("ami.in2p3.fr", "/AMI/servlet/net.hep.atlas.Database.Bookkeeping.AMI.Servlet.FrontEnd", 443);

	std::map<std::string, std::string> arguments;

	arguments.insert(std::pair<std::string, std::string>(
		"%MY_ARG1_NAME%",
		"%MY_ARG1_VALUE%"
	));

	arguments.insert(std::pair<std::string, std::string>(
		"%MY_ARG2_NAME%",
		"%MY_ARG2_VALUE%"
	));

	arguments.insert(std::pair<std::string, std::string>(
		"AMIUser",
		"%MY_LOGIN%"
	));

	arguments.insert(std::pair<std::string, std::string>(
		"AMIPass",
		"%MY_PASSWORD%"
	));

	std::string result = client.execute(
		"%MY_COMMAND%",
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

```
