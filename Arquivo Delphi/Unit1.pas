unit Unit1;

interface

uses
  System.SysUtils, System.Types, System.UITypes, System.Classes,
  System.Variants,
  FMX.Types, FMX.Controls, FMX.Forms, FMX.Graphics, FMX.Dialogs,
  IdBaseComponent, IdComponent, IdUDPBase, IdUDPClient, FMX.StdCtrls,
  FMX.Controls.Presentation, FMX.Edit, FMX.ListBox, FMX.Objects;

type
  TForm1 = class(TForm)
    IdUDPClient1: TIdUDPClient;
    ComboBox1: TComboBox;
    Label1: TLabel;
    Button3: TButton;
    Edit1: TEdit;
    Edit2: TEdit;
    Label2: TLabel;
    Label3: TLabel;
    Label4: TLabel;
    GroupBox1: TGroupBox;
    Button2: TButton;
    Button1: TButton;
    GroupBox2: TGroupBox;
    Rectangle1: TRectangle;
    Rectangle2: TRectangle;
    procedure Button3Click(Sender: TObject);
    procedure ComboBox1Change(Sender: TObject);
    procedure Rectangle2Click(Sender: TObject);
    procedure Rectangle1Click(Sender: TObject);
    procedure Button1Click(Sender: TObject);
    procedure Button2Click(Sender: TObject);
  private
    procedure VerificaStatusI_O;
    procedure conexao;
    procedure acionar(acao: Char);
    { Private declarations }
  public
    { Public declarations }
  end;

var
  Form1: TForm1;
  port: integer;
  retorno: string;

implementation

{$R *.fmx}

procedure TForm1.VerificaStatusI_O;
begin
  IdUDPClient1.Send('DR' + FormatFloat('00', port));
  retorno := IdUDPClient1.ReceiveString(700);
  if retorno = 'LOW' then
  begin
    Button1.TextSettings.FontColor := TColorRec.White;
    Button2.TextSettings.FontColor := TColorRec.Green;
  end
  else if retorno = 'HIGH' then
  begin
    Button1.TextSettings.FontColor := TColorRec.Green;
    Button2.TextSettings.FontColor := TColorRec.White;
  end;
end;

procedure TForm1.acionar(acao: Char);
begin
  IdUDPClient1.Send('DW' + FormatFloat('00', port) + acao);
end;

procedure TForm1.Button3Click(Sender: TObject);
begin
  conexao;
  ComboBox1Change(Sender);
end;

procedure TForm1.Button1Click(Sender: TObject);
begin
  acionar('+');
end;

procedure TForm1.Button2Click(Sender: TObject);
begin
  acionar('-');
end;

procedure TForm1.conexao;
begin
  IdUDPClient1.Host := Edit1.Text;
  IdUDPClient1.port := strToInt(Edit2.Text);
  IdUDPClient1.Connect;
  if IdUDPClient1.Connected then
  begin
    IdUDPClient1.Send('TC', nil);
    retorno := IdUDPClient1.ReceiveString(80);
    if retorno = 'TCOK' then
      Label4.Text := 'Conectado'
    else
      Label4.Text := 'Desconectado';
  end;
end;

procedure TForm1.ComboBox1Change(Sender: TObject);
begin
  port := strToInt(Copy(ComboBox1.Selected.Text, 0, 2));
end;

procedure TForm1.Rectangle1Click(Sender: TObject);
begin
  acionar('L');
end;

procedure TForm1.Rectangle2Click(Sender: TObject);
begin
  acionar('H');
end;

end.
