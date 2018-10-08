package model.text_unit.text.part;

import model.text_unit.text.TextUnitTypeEnum;
import model.text_unit.text.TextUnit;

public class PunctuationMark extends TextUnit {

    public static final String PUNCTUATION_MARK_NAME = "PUNCTUATION MARK";
    public static final String PATTERN = "([" + Sentence.DIVIDER + "]|[" + Word.DIVIDER + "])";
    private PunctuationMarkTypeEnum punctuationMarkType;


    public PunctuationMark(PunctuationMarkTypeEnum type) {
        super(type.toString() ,TextUnitTypeEnum.PUNCTUATION_MARK);
        punctuationMarkType = type;
    }

    @Override
    public String toString() {
        return punctuationMarkType.toString();
    }

}
