# File: tf_gui.py
#
# Windows GUI for tf_code utility
# Miloslav Číž, 2013

from tkinter import *
from tkinter.messagebox import *
from subprocess import *

root = Tk()

root.resizable(width=FALSE, height=FALSE)
root.wm_title("tf_code")

def is_positive_int(what): # checks if given string is positive integer
  try:
    int(what)
    return (int(what) >= 0)
  except ValueError:
    return False

def encode_click(event):   # encode click function
  if not is_positive_int(key_field.get()):
    showinfo("warning","Key must be a positive integer - it was set to 0.")
    key_field.delete(0,END)
    key_field.insert(0,"0")

  command = Popen("tf_code.exe" + " -e " + "\"" + alphabet_field.get() + "\" " + key_field.get(), stdout = PIPE, stdin = PIPE, shell = True)
  command.stdin.write(bytes(input_field.get(), 'UTF-8'));
  command.stdin.flush()
  command.stdin.close()
  output_field.delete(0,END)
  output_field.insert(0,command.stdout.read())
  if command.wait() != 0:
    showerror("Error","There was an error.")
    output_field.delete(0,END)

def decode_click(event):   # decode click function
  if not is_positive_int(key_field.get()):
    showinfo("warning","Key must be a positive integer - it was set to 0.")
    key_field.delete(0,END)
    key_field.insert(0,"0")

  command = Popen("tf_code.exe" + " -d " + "\"" + alphabet_field.get() + "\" " + key_field.get(), stdout = PIPE, stdin = PIPE, shell = True)
  command.stdin.write(bytes(input_field.get(), 'UTF-8'));
  command.stdin.flush()
  command.stdin.close()
  output_field.delete(0,END)
  output_field.insert(0,command.stdout.read())
  if command.wait() != 0:
    showerror("Error","There was an error.")
    output_field.delete(0,END)

alphabet_label = Label(root,text = "alphabet: ")
key_label = Label(root,text = "key (a number): ")
input_label = Label(root,text = "input: ")
output_label = Label(root,text = "output: ")
alphabet_field = Entry(root,width = 50)
key_field = Entry(root,width = 50)
input_field = Entry(root,width=50)
output_field = Entry(root,width=50)
button_encode = Button(root, text = "encode", width = 60)
button_decode = Button(root, text = "decode", width = 60)

key_field.insert(0,"0");
alphabet_field.insert(0,"abcdefghijklmnopqrstuvwxyz .,");
button_encode.bind("<Button-1>",encode_click);
button_decode.bind("<Button-1>",decode_click);

alphabet_label.grid(row = 0, column = 0)
key_label.grid(row = 1, column = 0)
input_label.grid(row = 2, column = 0)
output_label.grid(row = 3, column = 0)
alphabet_field.grid(row = 0, column = 1)
key_field.grid(row = 1, column = 1)
input_field.grid(row = 2, column = 1)
output_field.grid(row = 3, column = 1)
button_encode.grid(row = 4, column = 0, columnspan = 2)
button_decode.grid(row = 5, column = 0, columnspan = 2)

root.mainloop()
