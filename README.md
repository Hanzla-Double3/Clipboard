# Clipboard
Clipborad - access data quickly using cmd

# Configure

Create a folder, for example I created "D:/CustomEnvVar", and then add this path to enviroment variable.

Paste the code there. compile it using gcc, "gcc ./clipboard.c -o clipboard.exe". 

Create a file temp.json, populate it. In my case it was:

    {
  
      "ID": "12345678910",
  
      "Address": { 
  
        "Home" : "xyz street, house number n, City, State, Country",
    
        "Work" : "abc street, office number m, City, State, Country"
    
      }

    }

Now you can access work address my putting following command in cmd:

  "clipboard Address Work"

# Problem

I have taken its memory allocation seriously

No proper error handling 

I started working on my own json parser - Lost interest in it. It does the job.
