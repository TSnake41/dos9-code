��          �   %   �      p  e   q  >   �  R     r   i  "   �  c   �  >   c  G   �  ~   �  S   i  4   �  ?   �  (   2  _   [  ,  �    �  %   	  l   (	  T   �	  E   �	  e   0
  �   �
  (     �   E  �   �  1   q  g   �  �    z   �  ;   7  M   s  �   �  +   C  n   o  A   �  E      �   f  i   �  4   S  Z   �  -   �  p     "  �  7  �  ;   �  }     [   �  M   �  ~   A  �   �  3   N  �   �  �   .  <   �  u   
                                                                
                                       	                    Call a script or a label from the current context.
Usage: CALL [/e] [file] [:label] [parameters ...]
 Change console's color to the given code.
Usage: COLOR [code]
 Change current working directory.
Usage: CD  [[/d] path]
       CHDIR [[/d] path]
 Change positions of command-line parameters.
Usage: SHIFT [/start_number | /s[:]start_number] [/d[:]displacement]
 Clear console's screen
Usage: CLS
 Copy file(s) to another location.
Usage: COPY [/R] [/-Y | /Y] [/A[:]attributes] source destination
 Create an alias on a command.
Usage: ALIAS [/f] alias=command
 Create the given directory.
Usage: MD directory
       MKDIR directory
 Delete file(s).
Usage: DEL [/P] [/F] [/S] [/Q] [/A[:]attributes] name
       ERASE [/P] [/F] [/S] [/Q] [/A[:]attributes] name
 Display content of files.
Usage: MORE [/E] [/P] [/S] [/C] [/T[:]n] [+n] [file ...]
 Exit from the command prompt.
Usage: EXIT [/b code]
 Go to a label in a command script.
Usage: GOTO [:]label [file]
 Introduce a comment.
Usage: REM comment
 List files in a directory and in subdirectories.
Usage: DIR [/A[:]attributes] [/S] [/B] [path]
 Perform commands against items (files, strings, command output).
Usage: FOR %%A IN (string) DO (
          :: some commands
       )

       FOR /F [options] %%A IN (object) DO (
          :: some commands
       )

       FOR /L %%A IN (start,increment,end) DO (
          :: some commands
       )
 Perform commands on some conditions.
Usage: IF [/i] [NOT] string1==string2 (
          :: code to be ran
       )

       IF [/i] string1 cmp string2
          :: code to be ran
       )

       IF [NOT] [DEFINED | EXIST | ERRORLEVEL] object (
          :: code to be ran
       )
 Print a file.
Usage: TYPE [file ...]
 Print a message, or enable/disable commande echoing.
Usage: ECHO [OFF|ON]
       ECHO text
       ECHO.text
 Removes a directory.
Usage: RMDIR [/S] [/Q] directory
       RD [/S] [/Q] directory
 Renames a file.
Usage: REN file name.ext
       RENAME file name.ext
 Search occurence of string in a set of files.
Usage: FIND [/N] [/C] [/V] [/I] [/E] string [file ...]
 Set an environment variable.
Usage: SET variable=content
       SET /a[[:][i|f]] variable=expression
       SET /p variable=question
 Set console's title.
Usage: TITLE title
 Set options of the Dos9 command prompt.
Usage: SETLOCAL [ENABLEDELAYEDEXPANSION | DISABLEDELAYEDEXPANSION] [ENABLEFLOATS | DISABLEFLOATS] [CMDLYCORRECT | CMDLYINCORRECT]
 This help is voluntary limited help to fit in the binary. If you need to
see the full documentation, please type:

	HELP command Wait for a keystroke from the user.
Usage: PAUSE
 [Obsolete] Sets or Clears Extended CTRL+C checking on DOS system. (Do nothing)
Usage: BREAK [ON | OFF]
 Project-Id-Version: Dos9 0.9
Report-Msgid-Bugs-To: 
POT-Creation-Date: 2015-05-07 23:54+0200
PO-Revision-Date: 2014-04-15 18:03+0100
Last-Translator: DarkBatcher <darkbatcher@gmail.com>
Language-Team: French <darkbatcher@dos9.org>
Language: fr
MIME-Version: 1.0
Content-Type: text/plain; charset=UTF-8
Content-Transfer-Encoding: 8bit
Plural-Forms: nplurals=2; plural=(n > 1);
X-Generator: Poedit 1.5.7
X-Poedit-SourceCharset: UTF-8
 Appelle un script ou un étiquette depuis le contexte courant.
Usage: CALL [/e] [fichier] [:étiquette] [paramètres ...]
 Change la couleur de toute la console.
Usage: COLOR [code]
 Change le chemin courant.
Usage: CD [[/d] chemin]
       CHDIR [[/d] chemin]
 Change les positions des paramètres de ligne de commande.
Usage: SHIFT [/nombre_début | /s[:]nombre_début] [/d[:]déplacement
 Efface le contenu de la console
Usage: CLS
 Copie des fichiers vers un autre répertoire.
Usage: COPY [/R] [/-Y | /Y] [/A[:]attributs] source destination
 Crée un alias sur une commande.
Usage: ALIAS [/f] alias=command
 Crée un répertoire.
Usage: MD répertoire
       MKDIR répertoire
 Supprime des fichiers.
Usage: DEL [/P] [/F] [/S] [/Q] [/A[:]attributs] nom
       ERASE [/P] [/F] [/S] [/Q] [/A[:]attributes] nom
 Affiche le contenu d'un ou plusieur fichiers.
Usage: MORE [/E] [/P] [/S] [/C] [/T[:]n] [+n] [fichier...]
 Quitte l'invité de commande.
Usage: EXIT [/b code]
 Saute vers une étiquette dans un script de commande.
Usage: GOTO [:]étiquette [fichier]
 Introduit un commentaire.
Usage: REM comment
 Liste les fichiers d'un répertoire et de ses sous-répertoires.
Usage: DIR [/A[:]attributs] [/S] [/B] [chemin]
 Effectue une boucle de commandes sur des éléments.Usage: FOR %%A IN (chaîne) DO (
          :: des commandes
       )

       FOR /F [options] %%A IN (objets) DO (
          :: des commandes
       )

       FOR /L %%A IN (début,incrément,fin) DO (
          :: des commandes
       )
 Effectue des opérations conditionnelles de commandes.
Usage: IF [/i] [NOT] chaîne1==chaîne2 (
          :: code à exécuter
       )

       IF [/i] chaîne1 cmp chaîne2
          :: code à exécuter
       )

       IF [NOT] [DEFINED | EXIST | ERRORLEVEL] objet (
          :: code à exécuter
       )
 Affiche le contenu d'un fichier.
Usage: TYPE [fichier ...]
 Affiche un message, ou active/désactive l'affichage des commandes.
Usage: ECHO [OFF|ON]
       ECHO texte
       ECHO.texte
 Supprime un répertoire
Usage: RMDIR [/S] [/Q] répertoire
       RD [/S] [/Q] répertoire
 Renomme un fichier.
Usage: REN fichier nom.ext
       RENAME fichier nom.ext
 Recherche les occurences d'une chaîne dans un groupe de fichiers.
Usage: FIND [/N] [/C] [/V] [/I] [/E] chaîne [fichier ...]
 Affecte une variable d'environement.
Usage: SET variable=contenu
       SET /a[[:][i|f]] variable=expression
       SET /p variable=question
 Modifie le titre de la console.
Usage: TITLE titre
 Active ou désatice les options de Dos9.
Usage: SETLOCAL [ENABLEDELAYEDEXPANSION | DISABLEDELAYEDEXPANSION] [ENABLEFLOATS | DISABLEFLOATS] [CMDLYCORRECT | CMDLYINCORRECT]
 Cette aide est volontairement réduite pour tenir dans l'exécutable. Si vous avez
besoin de lire la documentation complète, merci d'entrer :

	HELP commande Attend que l'utilisateur appuie sur une touche
Usage: PAUSE
 [Obsolète] Active ou désactive le contrôle de CTRL+C sur les systèmesDOS. (Ne fait rien)
Usage: Break [ON | OFF]
 