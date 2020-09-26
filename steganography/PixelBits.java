
import java.awt.Color;
import java.util.*;
import java.io.*;
import javax.imageio.ImageIO;
import java.awt.image.BufferedImage;

public class PixelBits {

    private static int maxBytes = 0;

    public static void main(String[] args) {

        Scanner input = new Scanner(System.in);
        while (true) {
            System.out.println("Please, choose the option: ");
            System.out.println("1. Encode message");
            System.out.println("2. Decode message");
            System.out.println("3. Exit");
            int choice;
            choice = input.nextInt();
            switch (choice) {
                case 1: {
                    encodeMessage(input);
                    break;
                }
                case 2: {
                    decodeMessage(input);
                    break;
                }
                case 3: {
                    return;
                }
            }

        }
    }

    private static void decodeMessage(Scanner input) {
        System.out.print("Enter the input image file name (with .png extension): ");
        input.nextLine();
        String file = input.next();
        BufferedImage img;

        try {
            img = ImageIO.read(new File(file));
            // There is a method ImageIO.write() to create a 
            // picture file from a BufferedImage object

            int h = img.getHeight();
            int w = img.getWidth();

            // store each pixel's RGB values
            int[][] pixelData = new int[h * w][3];
            int[] rgb;

            int numberOfPixels = 0;
            for (int i = 0; i < h; i++) {
                for (int j = 0; j < w; j++) {
                    rgb = PixelData(img, j, i);
                    for (int k = 0; k < rgb.length; k++) {
                        pixelData[numberOfPixels][k] = rgb[k];
                    }

                    numberOfPixels++;
                }
            }
            int msgLength = msgLengthInt(pixelData);
            System.out.println("Message length: " + msgLength);
            String message = getMessageFromBits(pixelData, msgLength);

            System.out.println("Message is decoded!");
            System.out.println(message);

        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private static void encodeMessage(Scanner input) {
        System.out.print("Enter the input image file name (with .png extension): ");
        String file = input.next();
        BufferedImage img;

        try {
            img = ImageIO.read(new File(file));
            // There is a method ImageIO.write() to create a 
            // picture file from a BufferedImage object

            int h = img.getHeight();
            int w = img.getWidth();
            int totalpixel = h * w;
            System.out.println("\n Image height = " + h + " and Image width = " + w);
            System.out.println(" Total number of pixels = " + totalpixel);
            maxBytes = (totalpixel * 3) / 8;
            System.out.println("\n In each pixel you have 3 components, R-G-B.\n Hence you can embed 3 bits in each pixel.");
            System.out.println("\n Total number of bits that can be embedded in the picture = " + totalpixel * 3);
            System.out.println("\n Total number of ASCII characters that can be embedded = " + maxBytes);

            // store each pixel's RGB values
            int[][] pixelData = new int[h * w][3];
            int[] rgb;

            int numberOfPixels = 0;
            for (int i = 0; i < h; i++) {
                for (int j = 0; j < w; j++) {
                    rgb = PixelData(img, j, i);
                    for (int k = 0; k < rgb.length; k++) {
                        pixelData[numberOfPixels][k] = rgb[k];
                    }

                    numberOfPixels++;
                }
            }
            System.out.println("Enter the message to encode: ");
            input.nextLine();
            String message = input.nextLine();

            // 16 is the length of length of the message
            // i have to store in the beginning the length of the message, 
            // so first 16 bits will store integer, which is the length of the message, 
            // and length of this intege is 16 bits or 2 bytes, maximum number it can store is 2^16 -1
            if (message.length() + 16 > maxBytes) {
                System.out.println("Error. The message is too long for this picture");
                return;
            }
            int msgBits[] = messageBits(message);
            // firstly get the length of the message converted to binary
            int lengthBits[] = msgLengthBits(message.length());
            // put the length into the image
            for (int i = 0; i < 16; i++) {
                pixelData[i / 3][i % 3] &= ~(1);
                pixelData[i / 3][i % 3] |= (lengthBits[i]);
            }
            //first 16 bits are already written, so start from 16
            for (int i = 16; i < msgBits.length + 16; i++) {
                pixelData[i / 3][i % 3] &= ~(1);
                pixelData[i / 3][i % 3] |= msgBits[i - 16];
            }

            System.out.println("Message is encoded!");
            System.out.println("Enter the filename to write in: (without extension!)");
            String fileOut = input.next();
            File outImageFile = new File(fileOut + ".png");
            ImageIO.write(updatePixelData(img, w, h, pixelData), "png", outImageFile);
            System.out.println("Success!");
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private static int[] PixelData(BufferedImage img, int x, int y) {
        int argb = img.getRGB(x, y);
        int[] rgb = new int[3];
        rgb[0] = (argb >> 16) & 0xff; //Red
        rgb[1] = (argb >> 8) & 0xff;  // Green
        rgb[2] = (argb) & 0xff;       // Blue

        // 0xff is Hex for 255 and used as a mask to get last 8 bits of a bit string
        // Remove the comment from the next line if you want to display RGB values for all pixels.
        // Caution: There can be too many pixels to display. So use it if absolutely necessary.
        // System.out.println("Red: " + rgb[0] + ", Green: " + rgb[1] + ", Blue: " + rgb[2]);
        return rgb;
    }

    private static BufferedImage updatePixelData(BufferedImage img, int w, int h, int[][] pixelData) {
        for (int i = 0; i < h; i++) {
            for (int j = 0; j < w; j++) {
                int rgb = new Color(pixelData[i * w + j][0], pixelData[i * w + j][1], pixelData[i * w + j][2]).getRGB();
                img.setRGB(j, i, rgb);
            }
        }
        return img;
    }

    private static int[] messageBits(String message) {
        byte[] bytes = new byte[1];
        try {
            bytes = message.getBytes("UTF-8");
        } catch (UnsupportedEncodingException e) {
            e.printStackTrace();
        }
        //since each byte is 8 bits
        int[] bits = new int[bytes.length * 8];
        int c = 0;
        for (byte b : bytes) {
            int val = b;
            for (int i = 0; i < 8; i++) {
                int bit = (val & 128) == 0 ? 0 : 1;
                bits[c] = bit;
                c++;
                val <<= 1;
            }
        }
        
        return bits;
    }

    private static int[] msgLengthBits(int length) {
        int bits[] = new int[16];
        for (int i = 0; i < 16; i++) {
            bits[i] = 0;
        }
        int c = 0;
        while (length != 0) {
            bits[c] = length % 2;
            length /= 2;
            c++;
        }
        //the order of bits will be reversed, but it doesn't matter
        return bits;
    }

    private static int msgLengthInt(int[][] pixelData) {
        int length = 0;
        for (int i = 0; i < 16; i++) {
            length += (pixelData[i / 3][i % 3] & 1) * Math.pow(2, i);
        }
        return length;
    }

    private static byte binToInt(int[] arr) {
        byte n = 0;
        for (int i = 0; i < 8; i++) {
            n += Math.pow(2, 7-i) * arr[i];
        }
        return n;
    }

    private static String getMessageFromBits(int[][] pixelData, int l) {
        String str = "";
        int[] bits = new int[l * 8];
        byte bytes[] = new byte[l];
        for (int i = 16; i < l * 8 + 16; i++) {
            bits[i - 16] = pixelData[i / 3][i % 3] & 1;
        }
        for (int i = 0; i < l; i++) {
            int tmp[] = new int[8];
            for (int k = 0; k < 8; k++) {
                tmp[k] = bits[i * 8 + k];
            }
            bytes[i] = binToInt(tmp);
        }
        try {
            str = new String(bytes, "UTF-8");
        } catch (UnsupportedEncodingException e) {
            e.printStackTrace();
        }
        return str;
    }
}
