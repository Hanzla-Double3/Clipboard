# Clipboard
Clipborad - access data quickly using cmd

I would have done it in C++ or python to make it easier and consequently efficient, But I am starting to like c and trying to learning it better

# Configure

Create a folder, for example I created "D:/CustomEnvVar", and then add this path to enviroment variable in Path entry for user.

Paste code file in that directory, compile it using gcc, "gcc ./clipboard.c -o clipboard.exe". 

Create a file temp.json in same directory or change the filename in code, populate it. In my case it was:

    {
        "ID": "12345678910",
        "Address": 
        { 
            "Home" : "xyz street, house number n, City, State, Country",
            "Work" : "abc street, office number m, City, State, Country"
        }
    }

Make sure to change path in code too. It's absolute path. 

Now you can access work address my putting following command in cmd:

  "clipboard Address Work"

# Problem

I haven't taken its memory allocation seriously

No proper error handling 

I started working on my own json parser - Lost interest in it. It does the job.
