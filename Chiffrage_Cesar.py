#Chiffrage de Jules César
import string 
#traiter les cas où il y a des lettres en majuscules ou bien des caractères spéciaux
#entrée clavier
message=input("entrez une message : ")
while message != "quit":
    n=int(input("choisir une clée de chiffrage entre 1 et 6:  "))

    #test de l'exactitude de la clé
    while n<1 or n>6:
        print("Erreur, clée incorrecte")
        n=int(input("choisir une autre clée de chiffrage entre 1 et 3:  "))
    print(n)

    print(message,"->")
    alphabt=list(string.ascii_lowercase)
    res=""
    for i in range(len(message)):
        for j in range(26):
            if message[i]==alphabt[j]:
                if j+n >=26: #pour les lettre l'alphabet dont la clé est assez grande pour depasser 
                    res=res+alphabt[0+n]
                else:
                    res=res+alphabt[j+n]

    print("message chiffré : ",res)
    message=input("entrez une message : ")
print("fin")