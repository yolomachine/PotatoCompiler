def find_longest_word(word_list):
    longest_word = ''
    for word in word_list:
        if len(word) > len(longest_word):
            longest_word = word
    return longest_word

rw = [
	'"absolute"',
	'"and"',
	'"array"',
	'"asm"',
	'"begin"',
	'"break"',
	'"case"',
	'"const"',
	'"constructor"',
	'"continue"',
	'"destructor"',
	'"div"',
	'"do"',
	'"downto"',
	'"else"',
	'"end"',
	'"false"',
	'"file"',
	'"for"',
	'"function"',
	'"goto"',
	'"if"',
	'"implementation"',
	'"in"',
	'"inline"',
	'"interface"',
	'"label"',
	'"mod"',
	'"nil"',
	'"not"',
	'"object"',
	'"of"',
	'"packed"',
	'"procedure"',
	'"program"',
	'"record"',
	'"repeat"',
	'"set"',
	'"shl"',
	'"shr"',
	'"string"',
	'"then"',
	'"to"',
	'"true"',
	'"type"',
	'"unit"',
	'"until"',
	'"uses"',
	'"var"',
	'"while"',
	'"with"',
	'"xor"',
]

maxlen = len(find_longest_word(rw))
for x in sorted(rw):
	print(''.join(('    { ', x, ',', ' ' * (maxlen - len(x) + 4) , 'Token::Class::ReservedWord },')))