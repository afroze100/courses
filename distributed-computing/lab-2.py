def task1(directory):

    directory = "D:\SEECS\Lectures\Distributed Computing\Assignment/python_ass_afroze_310.txt"
    import string

    f=open(directory,"r")
    text=f.read()
    text=text.lower()

    for punc in string.punctuation:
        text=text.replace(punc,' ') ##might make some extra words like "s" which would be separated by apostrophe
        
    list_text=string.split(text)
    
    return list_text
    
    
#########################################


def task2():

    list_text=task1(1) ## the return value of previous function
    import string
    text=string.join(list_text)

    dict_words={}
    for element in list_text:
        dict_words[element]=0

    for word in dict_words.keys():
        point=0
        while(1):
            point=string.find(text,word,point)
            point=point+1 
            if(point==0): ##goes into this if word is not found in text
                break
            else:
                dict_words[word]+=1 ##if word is found it's count goes up

            check=0
            for item in string.lowercase: ##checks if the matching text is a complte word ora portion of the word
                if ((point+len(word)-1)<len(text)): ##doest go into this if, if we are at the end of the text
                    if text[point+len(word)-1]==item: ##goes into this if, if there is a lowercase english character right after the matching term
                        check=1                       ##this shows that the matchig phrase is not a complete word but a part of a word

                if point!=1:              ##check if it is not a small phrase at the ending of a large word like "s" often comes at end
                    if text[point-2]==item:
                        check=1

            if(check==1):
                dict_words[word]=dict_words[word]-1 ##if it is not a complete word then the count of the word in dictionary is decremented


    return dict_words
        

##########################################


def task3():

    dict_words=task2()
    import string

    list_items=dict_words.items()
    list_sorted=[]

    for index in range(0, len(list_items)):
        a=list_items[index][1]
        b=list_items[index][0]
        list_sorted.append((a,b))

    list_sorted.sort(reverse=True)

    list_frequent=list_sorted[0:20]


    return list_frequent


#########################################


class task4:

    def __init__(self,directory="python_ass_afroze_310.txt"):

        self.directory=directory

        

    def getFrequentWord(self,rank):

        import string
        direc=self.directory
        f=open(direc,"r")
        str_text=f.read()
        str_text=str_text.lower()

        for punc in string.punctuation:
            str_text=str_text.replace(punc,' ')
            
        list_text=string.split(str_text)

        dict_words={}
        for element in list_text:
            dict_words[element]=0
        word_list=dict_words.keys()
        word_list.sort()

        for word in dict_words.keys():
            point=0
            while(1):
                point=string.find(str_text,word,point)
                point=point+1
                if(point==0):
                    break
                else:
                    dict_words[word]+=1

                check=0
                for item in string.lowercase:
                    if ((point+len(word)-1)<len(str_text)):
                        if str_text[point+len(word)-1]==item:
                            check=1

                    if point!=1:              ##check if it is not a small phrase at the ending of a large word like "s" often comes at end
                        if str_text[point-2]==item:
                            check=1


                if(check==1):
                    dict_words[word]=dict_words[word]-1

        
        list_items=dict_words.items()
        list_sorted=[]
        for index in range(0, len(list_items)):
            a=list_items[index][1]
            b=list_items[index][0]
            list_sorted.append((a,b))
        list_sorted.sort(reverse=True)

        rank_list=[]
        rank_count=0
        rank_present=0
        rank_past=0
        for element in list_sorted:  ##for making the rank of words with same count as equal
            rank_present=element[0]
            if (rank_present == rank_past):
                rank_count-=1
            rank_count+=1
            rank_list.append(rank_count)
            rank_past=rank_present
            
        words=[]    
        for index in range(0,len(rank_list)):
            if (rank_list[index]==rank):
                words.append(list_sorted[index])

        return words
        


    def getUniqueWords(self):

        import string
        direc=self.directory
        f=open(direc,"r")
        str_text=f.read()
        str_text=str_text.lower()

        for punc in string.punctuation:
            str_text=str_text.replace(punc,' ')
            
        list_text=string.split(str_text)

        dict_words={}
        for element in list_text:
            dict_words[element]=0
        word_list=dict_words.keys()
        word_list.sort()

        return word_list


    def getWordRank(self,word_ranked):

        import string
        direc=self.directory
        f=open(direc,"r")
        str_text=f.read()
        str_text=str_text.lower()

        for punc in string.punctuation:
            str_text=str_text.replace(punc,' ')
            
        list_text=string.split(str_text)

        dict_words={}
        for element in list_text:
            dict_words[element]=0
        word_list=dict_words.keys()
        word_list.sort()

        for word in dict_words.keys():
            point=0
            while(1):
                point=string.find(str_text,word,point)
                point=point+1
                if(point==0):
                    break
                else:
                    dict_words[word]+=1

                check=0
                for item in string.lowercase:
                    if ((point+len(word)-1)<len(str_text)):
                        if str_text[point+len(word)-1]==item:
                            check=1

                    if point!=1:              ##check if it is not a small phrase at the ending of a large word like "s" often comes at end
                        if str_text[point-2]==item:
                            check=1

                if(check==1):
                    dict_words[word]=dict_words[word]-1
                    

        list_items=dict_words.items()
        list_sorted=[]
        for index in range(0, len(list_items)):
            a=list_items[index][1]
            b=list_items[index][0]
            list_sorted.append((a,b))
        list_sorted.sort(reverse=True)

        rank_list=[]
        rank_count=0
        rank_present=0
        rank_past=0
        for element in list_sorted:
            rank_present=element[0]
            if (rank_present == rank_past):
                rank_count-=1
            rank_count+=1
            rank_list.append(rank_count)
            rank_past=rank_present
            
        rank=-1
        for index in range (0, len(list_sorted)):
            if list_sorted[index][1]==word_ranked:
                rank = rank_list[index]


        return rank
