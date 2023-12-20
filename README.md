# neural-parser
This is a language for describing a single FFNN (feed forward neural network)
or an entity of FFNN (an FFNNE) connected together, train them
and tess them. It does a lot to the background and made it
easier for me to automate the creation of FFNN/FFNNE and assess them.

It is written in perl.

Over the years I have added functionality for manipulating data files
for training and testing the networks. Most importantly, I have added
support for using Support Vector Machines (SVM) and comparing to
the neural networks.
SVM programs are from Libsvm,
http://www.csie.ntu.edu.tw/~cjlin/libsvm, by
Chih-Chung Chang and Chih-Jen Lin).

This is my work. You can use and distribute it as you wish but NOT
for commercial purposes. Use it at your own risk!

# build instructions:
The preferred way to install is to dowmload the tarball distribtution
from this repository (https://raw.githubusercontent.com/hadjiprocopis/neural-parser/master/neuralparser-5.0.tar.gz)
```
tar xvzf neuralparser-5.0.tar.gz
cd neuralparser-5.0
./configure && make clean && make all
```

# how to use this language and tools provided
Here is a first script to start with:
```
SIN = CreateSingle {
# this is a plain, monolithic feed forward neural network
	SingleType = FFNN;
# oh this is deeeeep neeeet!
#	Arch = 14 53 139 73 31 3;
	Arch = 14 31 3;
# weights file
	Weights = W_SINGLE;
# output sigmoid?
	Sigmoid = Yes;
# this is a classification task
       NumOutputClasses = 2;
}
$ # <<< end of file
```
Let us save this script to a file called ```create.np```.

Now let's train it:
```
# the training file
A_FILE = OpenFileObject {
	Filename = training.txt;
}
IncludeFile {
	# read and execute the creation of the neural network
	# saved as SIN
	Filename = create.np;
}
# train the neural network
TrainSingle {
	Obj  = SIN;
	InpFileObj  = A_FILE;
	Iters = 1000;
	Beta = 0.1;
	Lamda = 0.0;
	Seed = 1234;
	# in order to observe evolution of weights we can save them occasionally
	SaveWeightsEveryNIterations = 100;
	# print progress report every so often
	ShowProgressEveryNIterations = 100;
	# to this log file
	ProgressFilename = progress.txt;

	UniqueWeightsFile = no;
	Silent = Yes;
}
# tell us that training has finished, it can also send an email...
SendInformation {
	OutFileName = finished.txt;
	Message = @ Single FFNN;
	Obj = SIN;
}
$
```
Fine, so let's save this file to `train.np` and let's proceed
to testing (assess the learning performance) of the trained
neural network with the following script:
```
INPUT = OpenFileObject{ Filename = testing.txt; }
IncludeFile {				     
	Filename = create.np;
}
TestSingle {
	Obj = SIN;
	InpFileObj  = INPUT;
	OutFileName = result.txt;
}
$
```

Now that we have the scripts we can call the interpreter (```np```)
to do the hard work. Note that you should already had installed
NNengine (from https://github.com/hadjiprocopis/NNengine) which
provides the Neural Network executables.

And here we are doing the training
```
np -clog training.clog -log training.log train.np
```

And here is the testing
```
np -clog training.clog -log training.log test.np
```

Now, there is a lot more to this. For example
input data manipulation and connecting
networks together or letting ```np``` create
a network according to spec.

There is a manual accompanying this software
which will give you more details.

Any questions or help please drop me a line.

author: Andreas Hadjiprocopis<br/>
andreashad2@gmail.comn (ex livantes@soi.city.ac.uk)<br/>
http://nfkb.scienceontheweb.net (ex http://soi.city.ac.uk/~livantes)<br/>

