#!/usr/bin/env python

import serial, time

from optparse import OptionParser

class Klugephrase:

    def connect(self, port):
        self.port = port
        self.connection = serial.Serial(port, 9600, timeout=3)
        time.sleep(8)
    
    def clear(self):
        command = "x\n"
        self.connection.write(command)
        self.connection.flushOutput()
        time.sleep(.5)

    def add_category(self, title):
        if len(title) > 15:
            raise Exception
        command = "c%s\n" % title
        self.connection.write(command)
        self.connection.flushOutput()
        time.sleep(.5)

    def add_word(self, category_index, word):
        if len(word) > 15:
            raise Exception
        command = "%d %s\n" % (category_index, word)
        self.connection.write(command)
        self.connection.flushOutput()
        time.sleep(.5)

    def close(self):
        self.connection.close()

def parse_file(filename):
    f = open(filename)
    data = {}
    category = None

    for line in f:
        line = line.strip()
        if not line:
            #it's blank
            continue
        if line.startswith("#"):
            continue
        if line.startswith("[") and line.endswith("]"):
            #it's a category
            category = line[1:-1]
            if len(category) <= 15:
                data[category] = []
            else:
                raise Exception, "Line too long: %s" % line
        else:
            #it's a word, append it to the current category

            if len(line) <= 15:
                data[category].append(line)
            else:
                raise Exception, "Line too long: %s" % line

    return data

def parse_cli():
    usage = """usage: %prog [options]
    
    Program attached klugephrase device with wordlists.
    
    Remember to format the wordlist with one word per line, with category
    headers enclosed in square brackets.  The maximum length of any word or
    category header is currently 15 characters.
    
    Example:
        [The Office]
        Jim Halpert
        Michael Scott
        ...
        [BSG]
        Starbuck
        Caprica
        Apollo
        Galactica

    Remember to hold down the programming buttons on the device after running
    this program to activate USB mode."""

    parser = OptionParser(usage)
    parser.add_option("-f", "--file", dest="filename",
                      help="read data from FILENAME")
    parser.add_option("-p", "--port", dest="port",
                      help="use serial port PORT")
    parser.add_option("-v", "--verbose",
                      action="store_true", dest="verbose", default=True)
    parser.add_option("-q", "--quiet",
                      action="store_false", dest="verbose")
    parser.add_option("--parse-only", dest="parse_only", action="store_true",
                      help="""Parse FILE only.  Do not connect or program
                      device.""")
    (options, args) = parser.parse_args()
    if not options.port and not options.parse_only:
        parser.err("port must be specified.")
    if not options.filename:
        parser.err("file must be specified.")
    return options, args

def main():

    options, args = parse_cli()

    data = parse_file(options.filename)
    if options.verbose:
        print "Parsing complete."
    if options.parse_only:
        for category in data:
            print category
            for word in data[category]:
                print "\t", word
        return

    kp = Klugephrase()
    if options.verbose:
        print "Connecting to device."
    kp.connect(options.port)
    if options.verbose:
        print "Clearing memory."
    kp.clear()
    print data
    for index, category in enumerate(data):
        if options.verbose:
            print "Adding category: %s" % category
        kp.add_category(category)
        for word in data[category]:
            if options.verbose:
                print "Adding word: %s" % word
            kp.add_word(index, word)
    if options.verbose:
        print "Done"
    kp.close()
    

if __name__ == "__main__":
    main()
