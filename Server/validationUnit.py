import os

content = []

def readFile(filename):
        global content
        if (len(content) != 0): content = []
        with open(filename, 'r') as f:
                for line in f:
                        content.append(line.replace('\n','').lower())
                f.close()
        return content

def validate(someId):
        global content
        if (len(content) == 0): content = readFile('/home/ivan/iot/validationData.txt')
        return someId.lower() in content