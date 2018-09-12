package model;

public class Syrup extends Medicine {

    private ColorEnum color;

    public ColorEnum getColor() {
        return color;
    }

    enum ColorEnum {
        RED {
            public String toString() {
                return "red";
            }
        },
        GREEN {
            public String toString() {
                return "green";
            }
        },
    }

    @Override
    public void someWork() {

    }
}
