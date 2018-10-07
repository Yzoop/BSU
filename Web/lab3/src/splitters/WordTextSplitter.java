package splitters;

import model.text_unit.code.CodeLine;
import model.text_unit.text.TextUnit;
import model.text_unit.text.part.PunctuationMark;
import model.text_unit.text.part.PunctuationMarkTypeEnum;
import model.text_unit.text.part.Sentence;
import model.text_unit.text.part.Word;

import java.util.ArrayList;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class WordTextSplitter {

    public ArrayList<TextUnit> split(ArrayList<TextUnit> textUnits) {
        ArrayList<TextUnit> result = new ArrayList<>();
        for (TextUnit textUnit: textUnits) {
            if (textUnit.getClass() == Sentence.class){
                Matcher matcher = Pattern.compile(Word.PATTERN).matcher(textUnit.getValue());
                Sentence sentence = new Sentence(textUnit.getValue());
                while (matcher.find()) {
                    if (Pattern.compile(PunctuationMark.PATTERN).matcher(matcher.group()).matches()) {
                        PunctuationMark punctuationMark = new PunctuationMark(PunctuationMarkTypeEnum.COLON);
                        sentence.addPunctuationMark(punctuationMark);
                    } else {
                        Word word = new Word(matcher.group());
                        sentence.addWord(word);
                    }
                }
                result.add(sentence);
            } else {
                result.add(textUnit);
            }
        }
        return result;
    }

    @Override
    public String toString() {
        return "Splitting into words";
    }
}
